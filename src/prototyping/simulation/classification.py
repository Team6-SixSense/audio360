from pathlib import Path
import joblib
import numpy as np
import librosa


PACKAGE_ROOT = Path(__file__).resolve().parent
file_path = PACKAGE_ROOT / "bin" / "pca_lda.pkl"

def extract_mfcc_features(y, sr=16000, n_mfcc=26):
    mfcc = librosa.feature.mfcc(y=y, sr=sr, n_mfcc=n_mfcc)
    return np.concatenate([np.mean(mfcc, axis=1), np.std(mfcc, axis=1)])

def classify_audio(audio_path):
    data = joblib.load(file_path)
    scaler, pca, lda = data["scaler"], data["pca"], data["lda"]
    feat = extract_mfcc_features(audio_path)
    feat_scaled = scaler.transform([feat])
    feat_pca = pca.transform(feat_scaled)
    pred = lda.predict(feat_pca)[0]
    prob = lda.predict_proba(feat_pca)[0]

    pred_prob = np.max(prob)
    if (pred_prob < 0.9):
        return "Undetermined"
    else:     
        return pred


