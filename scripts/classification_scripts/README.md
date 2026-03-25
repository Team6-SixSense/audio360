# Classification Training Scripts

Utilities for producing the PCA+LDA classifier that runs on the embedded Audio360 runtime. Both scripts emit a `pca_lda.pkl` for Python use and a `matrices.txt` containing C++-friendly arrays (PCA projection/mean, LDA scalings/weights/intercepts, class order). Create `scripts/classification_scripts/bin/` before running if it does not exist.

## Common requirements
- Python 3.9+ with `numpy`, `scipy`, `scikit-learn`, `librosa`, `joblib`, `tqdm` (and `datasets[audio]` when pulling FSD50K). Install with `pip install numpy scipy scikit-learn librosa joblib tqdm "datasets[audio]"`.
- Audio sampled at 16 kHz is expected; resampling is handled internally.

## `final_training_esc50.py` — full dataset training
Trains a PCA+LDA model aligned with the embedded feature stack (16 kHz, 2048 FFT/hop, 13 mel filters/MFCCs, PCA=6 components) using ESC-50 and UrbanSound8K, with optional FSD50K augmentation.

**Key flags**
- `--features drill rain siren` (required) — class names, lowercase, matching dataset labels.
- `--data-dir /path/to/ESC` — root that contains the `meta/` and audio folders above (default: `../../ESC` relative to this file).
- `--max-per-class N` — cap clips per class for balance; also applied during 5-fold CV.
- `--include-fsd50k` plus optional `--splits train validation` / `--meta-files dev.csv eval.csv` / `--hf-token $HF_TOKEN` to pull extra clips.
- `--debug-frames` — control MFCC debug printing (`none | first | all | comma-separated indices`).

**Dataset layout**
- `--audio-root` (default: `./audio` beside this script)
- Inside: one subfolder per class (`drill/`, `rain/`, `siren/`, etc.) containing `.wav`, `.flac`, `.ogg`, `.mp3`, `.m4a`, or `.aac` files.

**Key flags**
- `--features drill rain siren` (required) — class folder names to include (ideally exactly three).
- `--max-per-class 5` — upper bound on clips per class (default 5).
- `--noise-scale 0.005` — Gaussian noise scale relative to audio std (set `0` to disable).
- `--save-path bin/pca_lda.pkl` — destination for the trained model; `matrices.txt` is written alongside.
