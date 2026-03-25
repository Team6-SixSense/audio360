"""
Training pipeline for ESC-50, UrbanSound8K, and optional FSD50K audio, aligned
with the embedded classifier settings.

Embedded settings (runtime_audio360):
- sample rate:           16000 Hz
- frame size (n_fft):    2048 samples, Hann window
- hop length:            2048 (no overlap)
- mel filters:           13, HTK=False, norm=None
- MFCCs kept:            13 (DCT on log-mel)
- PCA dims:              6
- LDA classes:           user-provided order (e.g., drill, rain, siren)
"""

import os
import argparse
import csv
from collections import defaultdict

import joblib
import librosa
import numpy as np
from scipy.fftpack import dct
from sklearn.decomposition import PCA
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis as LDA
from sklearn.metrics import accuracy_score
from sklearn.model_selection import StratifiedKFold
from tqdm import tqdm


# Embedded-aligned defaults
SR = 16000
N_FFT = 2048
HOP = 2048
WIN = 2048
N_MELS = 13
N_MFCC = 13
N_PCA = 6


def extract_mfcc_features(
    audio_source,
    sr=SR,
    n_mfcc=N_MFCC,
    n_fft=N_FFT,
    hop_length=HOP,
    win_length=WIN,
    window="hann",
    n_mels=N_MELS,
    fmin=0.0,
    fmax=None,
    center=False,  # kept for API compatibility; manual framing ignores it
    scale_to_int16=False,
    debug_frames="first",  # "none" | "first" | "all" | comma-separated list or iterable of indices
):
    """
    Replicates the embedded feature stack more explicitly:
    frame -> window -> rFFT -> power -> mel -> log -> DCT

    Notes:
    - Uses manual framing instead of librosa.stft so FFT scaling is easier to debug.
    - Debug printing is controllable via debug_frames: first (default), all,
      none, or a list of 0-based indices.
    - Tries a normalized power spectrum using window energy, which is a common
      source of mismatch vs embedded implementations.
    """
    try:
        from datasets.features._torchcodec import AudioDecoder  # type: ignore
    except Exception:
        AudioDecoder = None

    if AudioDecoder is not None and isinstance(audio_source, AudioDecoder):
        audio_source = {
            "array": audio_source.get_all_samples().data.cpu().numpy(),
            "sampling_rate": audio_source.get_samples_played_in_range(0, 0).sample_rate,
        }

    if isinstance(audio_source, dict) and "array" in audio_source:
        y = np.asarray(audio_source["array"], dtype=np.float32)
        orig_sr = audio_source.get("sampling_rate", sr)
    elif isinstance(audio_source, (np.ndarray, list, tuple)):
        y = np.asarray(audio_source, dtype=np.float32)
        orig_sr = sr
    else:
        y, orig_sr = librosa.load(audio_source, sr=None, mono=True)
        y = np.asarray(y, dtype=np.float32)

    if y.ndim > 1:
        y = y.mean(axis=0)

    if orig_sr != sr:
        y = librosa.resample(y, orig_sr=orig_sr, target_sr=sr)
        y = np.asarray(y, dtype=np.float32)

    if scale_to_int16:
        y = y * 32768.0

    # Ensure at least one full frame
    if len(y) < win_length:
        y = librosa.util.fix_length(y, size=win_length)

    # Window selection
    if window == "hann":
        window_vals = np.hanning(win_length).astype(np.float32)
    else:
        raise ValueError(f"Unsupported window: {window}")

    # Manual framing
    frames = []
    for start in range(0, len(y) - win_length + 1, hop_length):
        frame = y[start:start + win_length]
        frame_win = frame * window_vals
        frames.append(frame_win.astype(np.float32))

    if not frames:
        frame = librosa.util.fix_length(y, size=win_length)[:win_length]
        frame_win = frame * window_vals
        frames = [frame_win.astype(np.float32)]

    frames = np.stack(frames, axis=0)  # [num_frames, win_length]

    # rFFT per frame
    spec = np.fft.rfft(frames, n=n_fft, axis=1)
    mag = np.abs(spec).astype(np.float32)

    # Try normalized power spectrum to better match embedded scaling
    window_energy = float(np.sum(window_vals ** 2))
    power_spec = (mag ** 2).astype(np.float32)

    # librosa mel filter expects [n_mels, freq_bins]
    if fmax is None:
        fmax = sr / 2.0

    mel_filter = librosa.filters.mel(
        sr=sr,
        n_fft=n_fft,
        n_mels=n_mels,
        fmin=fmin,
        fmax=fmax,
        htk=True,
        norm=None,
    ).astype(np.float32)

    # Convert to [freq_bins, num_frames] for matrix multiply
    power_spec_t = power_spec.T  # [freq_bins, num_frames]
    mel_spec = mel_filter @ power_spec_t  # [n_mels, num_frames]

    log_mel = np.log(mel_spec + 1e-10)

    mfcc = dct(log_mel, type=2, axis=0, norm="ortho")[:n_mfcc]

    # Debug: frame-level stats aligned with the STFT path
    def _resolve_debug_indices(raw):
        if raw is None or raw is False or raw == "none":
            return []
        if raw == "all":
            return list(range(frames.shape[0]))
        if raw == "first":
            return [0]
        if isinstance(raw, str):
            raw = [p.strip() for p in raw.split(",") if p.strip()]
        if isinstance(raw, (list, tuple, np.ndarray)):
            try:
                idxs = [int(v) for v in raw]
            except Exception:
                return []
        else:
            idxs = [int(raw)]

        idxs = sorted({i for i in idxs if 0 <= i < frames.shape[0]})
        return idxs

    for idx in _resolve_debug_indices(debug_frames):
        debug_frame = frames[idx]
        max_abs = float(np.max(np.abs(debug_frame)))
        rms = float(np.sqrt(np.mean(debug_frame ** 2)))
        first_10 = debug_frame[:10]
        power_sum = float(np.sum(power_spec[idx]))

        np.set_printoptions(threshold=np.inf)

        # print(f"Frame {idx} (windowed) max |sample|:", max_abs)
        # print(f"Frame {idx} (windowed) RMS:", rms)
        # print(f"Frame {idx} (windowed) first 10 samples:")
        # print(first_10)
        # print(f"Frame {idx} sum of power across all bins:", power_sum)
        # print(f"FFT power spectrum for frame {idx} (pre-mel):")
        # print(power_spec[idx])

        print(f"Mel vector for frame {idx} (pre-log):")
        print(mel_spec[:, idx])

        print(f"Log-mel vector for frame {idx} (pre-DCT):")
        print(log_mel[:, idx])

        print(f"MFCC frame {idx}:")
        print(mfcc[:, idx])
        
            
    return {
        "mfcc": mfcc.astype(np.float32),
        "mel": mel_spec.astype(np.float32),
        "log_mel": log_mel.astype(np.float32),
    }

    return mfcc.astype(np.float32)


def transform_pca_frames(mfcc, pca):
    return pca.transform(mfcc.T)


def build_frame_dataset(frames_by_clip, labels, indices):
    frames, frame_labels = [], []
    for idx in indices:
        clip_frames = frames_by_clip[idx]
        frames.append(clip_frames)
        frame_labels.append(np.full(clip_frames.shape[0], labels[idx]))
    return np.vstack(frames), np.concatenate(frame_labels)


def predict_clip(lda, frames_pca):
    preds = lda.predict(frames_pca)
    values, counts = np.unique(preds, return_counts=True)
    return values[np.argmax(counts)]


def limit_indices_per_class(labels, indices, max_per_class, seed=42):
    if max_per_class is None:
        return np.array(indices)
    if max_per_class <= 0:
        raise ValueError("max_per_class must be positive.")

    rng = np.random.RandomState(seed)
    indices = np.array(indices)
    keep_indices = []
    for cls in np.unique(labels[indices]):
        cls_indices = indices[labels[indices] == cls]
        if len(cls_indices) > max_per_class:
            cls_indices = rng.choice(cls_indices, size=max_per_class, replace=False)
        keep_indices.extend(cls_indices.tolist())
    return np.array(sorted(keep_indices))


def load_esc50_dataset(
    data_dir,
    selected_classes,
    sample_rate=SR,
    max_per_class=None,
    per_class_counts=None,
    debug_frames="first",
):
    """Load ESC-50 using the local esc50.csv and audio folder."""
    meta_path = os.path.join(data_dir, "meta", "esc50.csv")
    audio_dir = os.path.join(data_dir, "audio")
    if not os.path.isfile(meta_path):
        return [], []
    if per_class_counts is None:
        per_class_counts = defaultdict(int)
    all_X, all_y = [], []

    with open(meta_path, newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in tqdm(reader, desc="ESC-50"):
            cls = (row.get("category") or "").strip().lower()
            if cls not in selected_classes:
                continue
            if max_per_class is not None and per_class_counts[cls] >= max_per_class:
                continue
            fname = (row.get("filename") or "").strip()
            if not fname:
                continue
            path = os.path.join(audio_dir, fname)
            if not os.path.isfile(path):
                continue
            feats = extract_mfcc_features(path, sr=sample_rate, debug_frames=debug_frames)
            if feats is None:
                continue
            all_X.append(feats)
            all_y.append(cls)
            per_class_counts[cls] += 1

    if not all_X:
        return [], []
    return all_X, all_y


def load_urbansound_dataset(
    data_dir,
    selected_classes,
    sample_rate=SR,
    max_per_class=None,
    per_class_counts=None,
    debug_frames="first",
):
    """Load UrbanSound8K using the local UrbanSound8K.csv and urban_audio/ folder."""
    meta_path = os.path.join(data_dir, "meta", "UrbanSound8K.csv")
    audio_root = os.path.join(data_dir, "urban_audio")
    if not os.path.isfile(meta_path):
        return [], []
    if per_class_counts is None:
        per_class_counts = defaultdict(int)
    all_X, all_y = [], []

    with open(meta_path, newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in tqdm(reader, desc="UrbanSound8K"):
            cls = (row.get("category") or row.get("class") or "").strip().lower()
            if cls not in selected_classes:
                continue
            if max_per_class is not None and per_class_counts[cls] >= max_per_class:
                continue
            fold = str(row.get("fold") or "").strip()
            slice_file = (row.get("slice_file_name") or "").strip()
            if not fold or not slice_file:
                continue
            path = os.path.join(audio_root, f"fold{fold}", slice_file)
            if not os.path.isfile(path):
                continue
            feats = extract_mfcc_features(path, sr=sample_rate, debug_frames=debug_frames)
            if feats is None:
                continue
            all_X.append(feats)
            all_y.append(cls)
            per_class_counts[cls] += 1

    if not all_X:
        return [], []
    return all_X, all_y


def load_fsd50k_dataset(
    selected_classes,
    sample_rate=SR,
    max_per_class=None,
    splits=None,
    hf_token=None,
    hf_repo="Fhrozen/FSD50k",
    meta_files=None,
    per_class_counts=None,
    debug_frames="first",
):
    """Load FSD50K via Hugging Face using local metadata CSVs for labels."""
    try:
        from datasets import load_dataset, Audio  # type: ignore
    except ImportError as exc:
        raise ImportError("Please install the 'datasets' package: pip install \"datasets[audio]\"") from exc

    if per_class_counts is None:
        per_class_counts = defaultdict(int)

    def _detect_column(candidates, dataset):
        for name in candidates:
            if name in dataset.column_names:
                return name
        return None

    def _load_label_metadata(paths, splits_filter=None):
        lookup = {}
        splits_filter = None if splits_filter is None else {s.lower() for s in splits_filter}
        for path in paths or []:
            if not os.path.isfile(path):
                continue
            with open(path, newline="", encoding="utf-8") as f:
                reader = csv.DictReader(f)
                for row in reader:
                    fname = (row.get("fname") or "").strip()
                    if not fname:
                        continue
                    split = (row.get("split") or "").strip().lower()
                    if splits_filter and split and split not in splits_filter:
                        continue
                    labels = [lbl.strip().lower() for lbl in (row.get("labels") or "").split(",") if lbl.strip()]
                    if labels:
                        lookup[fname] = labels
        return lookup

    base = os.path.dirname(os.path.abspath(__file__))
    if meta_files is None:
        meta_files = [os.path.join(base, "dev.csv"), os.path.join(base, "eval.csv")]
    meta_lookup = _load_label_metadata(meta_files, splits)
    if not meta_lookup:
        return [], []

    selected_classes = [c.lower() for c in selected_classes]
    split_dict = load_dataset(hf_repo, token=hf_token)
    if splits:
        split_dict = {k: v for k, v in split_dict.items() if k in splits}
    if not split_dict:
        return [], []

    all_X, all_y = [], []
    for split_name, split_ds in split_dict.items():
        audio_col = _detect_column(["audio", "sound", "wav", "audio_file", "path", "filepath"], split_ds)
        label_col = _detect_column(["labels", "label", "class", "classes", "category", "categories"], split_ds)
        if audio_col is None or label_col is None:
            continue

        try:
            split_ds = split_ds.cast_column(audio_col, Audio(sampling_rate=sample_rate))
        except Exception:
            pass

        total = len(split_ds) if hasattr(split_ds, "__len__") else None
        for example in tqdm(split_ds, total=total, desc=f"FSD50K {split_name}"):
            audio_val = example[audio_col]
            path = getattr(audio_val, "_hf_encoded", {}).get("path") if hasattr(audio_val, "_hf_encoded") else None
            fname = os.path.splitext(os.path.basename(path))[0] if path else None
            if not fname or fname not in meta_lookup:
                continue

            labels = meta_lookup[fname]
            matched = [lbl for lbl in labels if lbl in selected_classes]
            if len(matched) == 0:
                continue
            target_label = matched[0]
            if max_per_class is not None and per_class_counts[target_label] >= max_per_class:
                continue

            feats = extract_mfcc_features(audio_val, sr=sample_rate, debug_frames=debug_frames)
            if feats is None:
                continue

            all_X.append(feats)
            all_y.append(target_label)
            per_class_counts[target_label] += 1

    if not all_X:
        return [], []
    return all_X, all_y


def train_pca_lda_model(
    data_dir,
    selected_classes,
    save_path,
    n_pca=N_PCA,
    max_per_class=None,
    sample_rate=SR,
    debug_frames="first",
    include_fsd50k=False,
    splits=None,
    hf_token=None,
    meta_files=None,
    hf_repo="Fhrozen/FSD50k",
):
    selected_classes = [c.lower() for c in selected_classes]
    per_class_counts = defaultdict(int)
    X_esc, y_esc = load_esc50_dataset(
        data_dir,
        selected_classes,
        sample_rate=sample_rate,
        max_per_class=max_per_class,
        per_class_counts=per_class_counts,
        debug_frames=debug_frames,
    )
    X_urb, y_urb = load_urbansound_dataset(
        data_dir,
        selected_classes,
        sample_rate=sample_rate,
        max_per_class=max_per_class,
        per_class_counts=per_class_counts,
        debug_frames=debug_frames,
    )
    X_fsd, y_fsd = [], []
    if include_fsd50k:
        X_fsd, y_fsd = load_fsd50k_dataset(
            selected_classes,
            sample_rate=sample_rate,
            max_per_class=max_per_class,
            splits=splits,
            hf_token=hf_token,
            hf_repo=hf_repo,
            meta_files=meta_files,
            per_class_counts=per_class_counts,
            debug_frames=debug_frames,
        )

    X = X_esc + X_urb + X_fsd
    y = np.array(y_esc + y_urb + y_fsd)
    if len(X) == 0:
        raise ValueError("No samples found in ESC-50, UrbanSound8K, or FSD50K for the specified classes.")

    print("\nComputing PCA on frames...")
    X_frames = np.concatenate([clip.T for clip in X], axis=0)
    if n_pca > X_frames.shape[1]:
        raise ValueError(f"n_pca ({n_pca}) must be <= n_mfcc ({X_frames.shape[1]}).")
    pca = PCA(n_components=n_pca)
    pca.fit(X_frames)
    frames_by_clip = [transform_pca_frames(clip, pca) for clip in X]

    lda = LDA()
    kfold = StratifiedKFold(n_splits=5, shuffle=True, random_state=42)
    class_acc, overall_acc = defaultdict(list), []

    print("\nPerforming 5-fold cross-validation...\n")
    for fold_idx, (train_idx, test_idx) in enumerate(kfold.split(np.zeros(len(y)), y), 1):
        train_idx_limited = limit_indices_per_class(y, train_idx, max_per_class, seed=42 + fold_idx)
        X_train, y_train = build_frame_dataset(frames_by_clip, y, train_idx_limited)
        lda.fit(X_train, y_train)
        preds = np.array([predict_clip(lda, frames_by_clip[i]) for i in test_idx])
        y_test = y[test_idx]
        acc = accuracy_score(y_test, preds)
        overall_acc.append(acc)

        for cls in selected_classes:
            mask = (y_test == cls)
            if np.sum(mask) > 0:
                class_acc[cls].append(accuracy_score(y_test[mask], preds[mask]))
        print(f"Fold {fold_idx}: Overall accuracy = {acc*100:.2f}%")

    print("\n===== Cross-validation results =====")
    print(f"Overall mean accuracy: {np.mean(overall_acc)*100:.2f}%")
    for cls in selected_classes:
        if cls in class_acc:
            mean_acc = np.mean(class_acc[cls]) * 100
            print(f"  {cls:20s}: {mean_acc:6.2f}%")

    final_idx = limit_indices_per_class(y, np.arange(len(y)), max_per_class, seed=42)
    X_all, y_all = build_frame_dataset(frames_by_clip, y, final_idx)
    lda.fit(X_all, y_all)
    joblib.dump({"pca": pca, "lda": lda}, save_path)
    print(f"\n💾 Final PCA+LDA model saved to {save_path}")

    def _chunked(seq, size):
        for i in range(0, len(seq), size):
            yield seq[i: i + size]

    def _format_cpp_values(values, formatter, per_line=13):
        values = list(values)
        if not values:
            return "{}"
        lines = []
        for chunk in _chunked(values, per_line):
            lines.append(", ".join(formatter(v) for v in chunk))
        return "{\n  " + ",\n  ".join(lines) + "\n}"

    def format_cpp_vector(values, per_line=13):
        values = np.ravel(values)
        return _format_cpp_values(values, lambda v: f"{v:.8f}", per_line=per_line)

    def format_cpp_string_vector(values, per_line=13):
        values = np.ravel(values)
        return _format_cpp_values(values, lambda v: f"\"{v}\"", per_line=per_line)

    matrices_path = os.path.join(os.path.dirname(save_path), "matrices.txt")
    with open(matrices_path, "w", encoding="utf-8") as f:
        f.write("PCA Projection Matrix:\n")
        f.write(f"{format_cpp_vector(pca.components_.T)}\n\n")  # 13 x 6 for MCU\n
        f.write("PCA Mean Vector:\n")
        f.write(f"{format_cpp_vector(pca.mean_)}\n\n")
        f.write("LDA projection matrix (scalings_):\n")
        f.write(f"{format_cpp_vector(getattr(lda, 'scalings_', []))}\n\n")
        f.write("LDA class weights (coef_):\n")
        f.write(f"{format_cpp_vector(lda.coef_)}\n\n")
        f.write("LDA intercepts:\n")
        f.write(f"{format_cpp_vector(lda.intercept_)}\n\n")
        f.write(f"Class order: {format_cpp_string_vector(lda.classes_)}\n")
    print(f"💾 Matrices saved to {matrices_path}")


def main():
    parser = argparse.ArgumentParser()
    base_dir = os.path.dirname(os.path.abspath(__file__))
    default_data_dir = os.path.abspath(os.path.join(base_dir, "..", "..", "ESC"))
    model_path = os.path.join(base_dir, "bin", "pca_lda.pkl")

    parser.add_argument("--features", nargs="+", required=True, help="List of class names to train on")
    parser.add_argument("--data-dir", default=default_data_dir, help="Path to ESC/UrbanSound root (with meta/*.csv)")
    parser.add_argument("--max-per-class", type=int, default=None, help="Cap clips per class for balance")
    parser.add_argument("--sample-rate", type=int, default=SR, help="Target sample rate (default 16 kHz)")
    parser.add_argument("--include-fsd50k", action="store_true", help="Also pull clips from FSD50K (Hugging Face)")
    parser.add_argument("--splits", nargs="+", default=None, help="FSD50K splits to use (e.g., train validation)")
    parser.add_argument("--meta-files", nargs="+", default=None, help="Paths to FSD50K metadata CSVs (dev/eval)")
    parser.add_argument("--hf-token", default=os.getenv("HF_TOKEN"), help="Hugging Face token if needed")
    parser.add_argument("--hf-repo", default="Fhrozen/FSD50k", help="Hugging Face repo id for FSD50K")
    parser.add_argument(
        "--debug-frames",
        default="first",
        help="Debug which frames: none | first | all | comma-separated indices (0-based)",
    )
    args = parser.parse_args()

    train_pca_lda_model(
        data_dir=args.data_dir,
        selected_classes=args.features,
        save_path=model_path,
        max_per_class=args.max_per_class,
        sample_rate=args.sample_rate,
        debug_frames=args.debug_frames,
        include_fsd50k=args.include_fsd50k,
        splits=args.splits,
        hf_token=args.hf_token,
        meta_files=args.meta_files,
        hf_repo=args.hf_repo,
    )


if __name__ == "__main__":
    main()
