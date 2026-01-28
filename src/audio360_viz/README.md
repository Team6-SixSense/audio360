# audio360_viz

Smart-glasses visualization (green-only) for SixSense.

## Build APK

```bash
cd src/audio360_viz
flutter pub get
flutter build apk --release
```

APK output:
`build/app/outputs/flutter-apk/app-release.apk`

## Install on glasses (APK)

1. Enable **Developer options** + **USB debugging** on the glasses.
2. Connect glasses to PC.
3. Install:

```bash
adb install -r build/app/outputs/flutter-apk/app-release.apk
```

## Data packet (USB)

4 bytes: `0xAA | classification | direction | priority`
