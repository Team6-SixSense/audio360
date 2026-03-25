"""
Train PCA+LDA from a tiny, local dataset: up to 5 audio files per class pulled
from ./audio/<class_name>/*.wav (or other common formats) inside pca_lda/.

Exports:
- pca_lda.pkl
- matrices.txt

Also exports:
- PCA Projection Matrix
- PCA Mean Vector
- LDA Mean Vector (xbar_)
- LDA projection matrix (scalings_)
- LDA 2D classifier weights
- LDA 2D classifier biases
- Class order

Change:
- Instead of fitting 2D weights/biases to old LDA scores with least squares,
  this trains a classifier directly on the 2D LDA coordinates using the labels.
- Adds light Gaussian background noise before feature extraction (configurable via
  --noise-scale, set to 0 to disable).
"""

import os
import argparse
import random
import numpy as np
import joblib
import librosa
from sklearn.decomposition import PCA
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis as LDA
from sklearn.linear_model import LogisticRegression

from final_training_esc50 import (
    extract_mfcc_features,
    transform_pca_frames,
    build_frame_dataset,
    SR,
    N_PCA,
)

SUPPORTED_EXTS = (".wav", ".flac", ".ogg", ".mp3", ".m4a", ".aac")


def _strip_silence(audio, sr, top_db=40, min_duration_sec=0.05):
    intervals = librosa.effects.split(audio, top_db=top_db, ref=np.max)
    if len(intervals) == 0:
        return np.array([], dtype=audio.dtype)
    kept = np.concatenate([audio[start:end] for start, end in intervals])
    min_samples = max(int(min_duration_sec * sr), 1)
    return kept if kept.size >= min_samples else np.array([], dtype=audio.dtype)


def _load_audio_without_silence(path, target_sr=SR):
    try:
        audio, sr = librosa.load(path, sr=target_sr, mono=True)
    except Exception:
        return None, None

    # Uncomment if you want silence stripping:
    # audio = _strip_silence(audio, sr)

    if audio.size == 0:
        return None, None
    return audio, sr


def _add_background_noise(audio, scale=0.005, rng=None):
    """Add light Gaussian noise to simulate background hiss."""
    if scale <= 0:
        return audio
    rng = rng or np.random.default_rng()
    std = np.std(audio)
    if std == 0:
        return audio
    noise = rng.normal(0.0, std * scale, size=audio.shape)
    return audio + noise


def load_local_folder_dataset(
    audio_root,
    selected_classes,
    sample_rate=SR,
    max_per_class=5,
    seed=42,
    noise_scale=0.005,
):
    rng = random.Random(seed)
    np_rng = np.random.default_rng(seed)
    X, y = [], []
    selected_classes = [c.lower() for c in selected_classes]

    for cls in selected_classes:
        cls_dir = os.path.join(audio_root, cls)
        if not os.path.isdir(cls_dir):
            continue

        files = [
            os.path.join(cls_dir, f)
            for f in os.listdir(cls_dir)
            if f.lower().endswith(SUPPORTED_EXTS) and os.path.isfile(os.path.join(cls_dir, f))
        ]

        if not files:
            continue

        rng.shuffle(files)

        for path in files[:max_per_class]:
            audio, orig_sr = _load_audio_without_silence(path, target_sr=sample_rate)
            if audio is None:
                continue

            audio = _add_background_noise(audio, scale=noise_scale, rng=np_rng)

            feats = extract_mfcc_features(
                {"array": audio, "sampling_rate": orig_sr},
                sr=sample_rate
            )['mfcc'][1:13]

            if feats is None or feats.size == 0 or feats.shape[1] == 0:
                continue

            X.append(feats)
            y.append(cls)

    return X, np.array(y)


def train_small(
    audio_root,
    selected_classes,
    save_path,
    n_pca=N_PCA,
    max_per_class=5,
    sample_rate=SR,
    noise_scale=0.005,
):
    X, y = load_local_folder_dataset(
        audio_root,
        selected_classes,
        sample_rate=sample_rate,
        max_per_class=max_per_class,
        noise_scale=noise_scale,
    )

    X, y = zip(*[
        (xi, yi)
        for xi, yi in zip(X, y)
        if xi is not None and xi.size > 0 and xi.shape[1] > 0
    ]) if len(X) else ([], [])

    X = list(X) if X else []
    y = np.array(y) if len(X) else np.array([])

    if len(X) == 0:
        raise ValueError(f"No audio found for classes {selected_classes} under {audio_root}")

    print(f"Loaded {len(y)} clips from {audio_root}")

    # Build frame-level MFCC data
    X_frames = np.concatenate([clip.T for clip in X], axis=0)

    if n_pca > X_frames.shape[1]:
        raise ValueError(f"n_pca ({n_pca}) must be <= n_mfcc ({X_frames.shape[1]}).")

    pca = PCA(n_components=n_pca)
    pca.fit(X_frames)

    # Each item becomes [num_frames_in_clip, n_pca]
    frames_by_clip = [transform_pca_frames(clip, pca) for clip in X]

    lda = LDA()
    all_idx = np.arange(len(y))
    X_all, y_all = build_frame_dataset(frames_by_clip, y, all_idx)
    print("TRAIN PCA sample:", X_all[:5])
    lda.fit(X_all, y_all)

    # 2D LDA coordinates
    Z_all = lda.transform(X_all)  # [N, 2] for 3 classes

    if Z_all.ndim != 2 or Z_all.shape[1] != 2:
        raise ValueError(
            f"Expected 2D LDA coordinates for 3-class problem, got shape {Z_all.shape}"
        )

    # Train classifier directly in 2D LDA space
    clf_2d = LogisticRegression(
        multi_class="multinomial",
        solver="lbfgs",
        max_iter=1000,
        random_state=42,
    )
    clf_2d.fit(Z_all, y_all)

    lda_2d_weights = clf_2d.coef_       # [num_classes, 2]
    lda_2d_biases = clf_2d.intercept_   # [num_classes]

    train_acc_2d = clf_2d.score(Z_all, y_all)

    print("LDA classes:", lda.classes_)
    print("LDA transformed shape:", Z_all.shape)
    print("2D classifier weights shape:", lda_2d_weights.shape)
    print("2D classifier biases shape:", lda_2d_biases.shape)
    print(f"2D classifier training accuracy: {train_acc_2d:.4f}")
    
    print("classes:", clf_2d.classes_)
    print("coef:\n", clf_2d.coef_)
    print("intercept:\n", clf_2d.intercept_)
    print("train acc:", clf_2d.score(Z_all, y_all))
    for cls in clf_2d.classes_:
        pts = Z_all[y_all == cls]
        print(cls, pts.mean(axis=0), pts.std(axis=0))

    joblib.dump(
        {
            "pca": pca,
            "lda": lda,
            "clf_2d": clf_2d,
            "lda_2d_weights": lda_2d_weights,
            "lda_2d_biases": lda_2d_biases,
        },
        save_path
    )
    print(f"\nModel saved to {save_path}")

    def _chunked(seq, size):
        for i in range(0, len(seq), size):
            yield seq[i:i + size]

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
        return _format_cpp_values(values, lambda v: f"{float(v):.8f}f", per_line=per_line)

    def format_cpp_string_vector(values, per_line=13):
        values = np.ravel(values)
        return _format_cpp_values(values, lambda v: f"\"{v}\"", per_line=per_line)

    matrices_path = os.path.join(os.path.dirname(save_path), "matrices.txt")
    with open(matrices_path, "w", encoding="utf-8") as f:
        f.write("PCA Projection Matrix:\n")
        f.write(f"{format_cpp_vector(pca.components_.T)}\n\n")

        f.write("PCA Mean Vector:\n")
        f.write(f"{format_cpp_vector(pca.mean_)}\n\n")

        f.write("LDA Mean Vector (xbar_):\n")
        f.write(f"{format_cpp_vector(getattr(lda, 'xbar_', []))}\n\n")

        f.write("LDA projection matrix (scalings_):\n")
        f.write(f"{format_cpp_vector(getattr(lda, 'scalings_', []))}\n\n")

        f.write("LDA 2D class weights:\n")
        f.write(f"{format_cpp_vector(lda_2d_weights)}\n\n")

        f.write("LDA 2D class biases:\n")
        f.write(f"{format_cpp_vector(lda_2d_biases)}\n\n")

        f.write("Original LDA class weights (coef_):\n")
        f.write(f"{format_cpp_vector(lda.coef_)}\n\n")

        f.write("Original LDA intercepts:\n")
        f.write(f"{format_cpp_vector(lda.intercept_)}\n\n")

        f.write(f"Class order: {format_cpp_string_vector(lda.classes_)}\n")

    print(f"Matrices saved to {matrices_path}")


def main():
    base_dir = os.path.dirname(os.path.abspath(__file__))
    audio_root = os.path.join(base_dir, "audio")
    model_path = os.path.join(base_dir, "bin", "pca_lda.pkl")

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--features",
        nargs="+",
        required=True,
        help="Class names (must match folder names under audio/)"
    )
    parser.add_argument(
        "--audio-root",
        default=audio_root,
        help="Path to root folder containing class subfolders"
    )
    parser.add_argument(
        "--sample-rate",
        type=int,
        default=SR,
        help="Target sample rate"
    )
    parser.add_argument(
        "--save-path",
        default=model_path,
        help="Where to write the trained model (pkl)"
    )
    parser.add_argument(
        "--max-per-class",
        type=int,
        default=5,
        help="Max clips per class to use"
    )
    parser.add_argument(
        "--noise-scale",
        type=float,
        default=0.005,
        help="Gaussian noise scale relative to audio std (0 to disable)"
    )
    args = parser.parse_args()

    train_small(
        audio_root=args.audio_root,
        selected_classes=args.features,
        save_path=args.save_path,
        max_per_class=args.max_per_class,
        sample_rate=args.sample_rate,
        noise_scale=args.noise_scale,
    )


if __name__ == "__main__":
    main()
