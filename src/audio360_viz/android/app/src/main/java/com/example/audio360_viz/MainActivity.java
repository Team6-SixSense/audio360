package com.example.audio360_viz;

import io.flutter.embedding.android.FlutterActivity;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.app.PendingIntent;
import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.os.ParcelFileDescriptor;
import androidx.annotation.NonNull;
import android.util.Log;

import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Arrays;

import io.flutter.embedding.android.FlutterActivity;
import io.flutter.embedding.engine.FlutterEngine;
import io.flutter.plugin.common.EventChannel;

public class MainActivity extends FlutterActivity {


    private static final String TAG = "AOA_DEBUG";
    private static final String CHANNEL = "com.audio360.aoa/stream";
    private static final String ACTION_USB_PERMISSION = "com.example.audio360_viz.USB_PERMISSION";
    private ParcelFileDescriptor fileDescriptor;
    private FileInputStream inputStream;
    private boolean isReading = false;
    private Thread readThread;

    private final Object pipeLock = new Object(); // Lock for thread safety
    private volatile boolean isClosing = false;   // Flag to prevent race conditions

    // Receiver to handle the "Allow USB?" popup response
    private final BroadcastReceiver usbReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (ACTION_USB_PERMISSION.equals(action)) {
                synchronized (this) {
                    UsbAccessory accessory = intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                        if (accessory != null) {
                            // Permission granted! Now we can open it.
                            openPipe(accessory);
                        }
                    } else {
                        System.err.println("Permission denied for accessory " + accessory);
                    }
                }
            }
        }
    };

    // Required to send events to Flutter
    private EventChannel.EventSink globalEvents;

    @Override
    public void configureFlutterEngine(@NonNull FlutterEngine flutterEngine) {
        super.configureFlutterEngine(flutterEngine);

        // Register the Broadcast Receiver for permissions
        IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            registerReceiver(usbReceiver, filter, Context.RECEIVER_NOT_EXPORTED);
        } else {
            registerReceiver(usbReceiver, filter);
        }

        new EventChannel(flutterEngine.getDartExecutor().getBinaryMessenger(), CHANNEL)
                .setStreamHandler(new EventChannel.StreamHandler() {
                    @Override
                    public void onListen(Object arguments, EventChannel.EventSink events) {
                        globalEvents = events;
                        findAndOpenAccessory();
                    }

                    @Override
                    public void onCancel(Object arguments) {
                        closeAccessory();
                        globalEvents = null;
                    }
                });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(usbReceiver);
    }

    private void findAndOpenAccessory() {
        UsbManager manager = (UsbManager) getSystemService(Context.USB_SERVICE);
        UsbAccessory accessory = null;

        // STRATEGY 1: Check if the app was auto-launched by the cable plug-in
        if (getIntent() != null) {
            accessory = getIntent().getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
        }

        // STRATEGY 2: If manual launch, scan the list of connected devices
        if (accessory == null) {
            UsbAccessory[] list = manager.getAccessoryList();
            if (list != null && list.length > 0) {
                // Just grab the first one (Usually there is only one)
                accessory = list[0];
            }
        }

        if (accessory != null) {
            if (manager.hasPermission(accessory)) {
                openPipe(accessory);
            } else {
                // If we don't have permission, we MUST ask for it or openAccessory will crash.
                PendingIntent permissionIntent = PendingIntent.getBroadcast(this, 0, new Intent(ACTION_USB_PERMISSION), PendingIntent.FLAG_IMMUTABLE);
                manager.requestPermission(accessory, permissionIntent);
            }
        } else {
            if (globalEvents != null) {
                globalEvents.error("NO_DEVICE", "No AOA device found. Is the STM32 connected?", null);
            }
        }
    }

    @Override
    protected void onNewIntent(@NonNull Intent intent) {
        super.onNewIntent(intent);
        Log.d(TAG, "onNewIntent triggered - checking for accessory");
        handleIntent(intent);
    }

    private void handleIntent(Intent intent) {
        if (intent != null && intent.getAction() != null) {
            Log.d(TAG, "Intent Action: " + intent.getAction());
        }

        UsbAccessory accessory = intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
        boolean fromScan = false;

        // Fallback: If intent didn't have it, scan for it
        if (accessory == null) {
            UsbManager manager = (UsbManager) getSystemService(Context.USB_SERVICE);
            UsbAccessory[] list = manager.getAccessoryList();
            if (list != null && list.length > 0) {
                accessory = list[0];
                fromScan = true;
            }
        }

        if (accessory != null) {
            Log.d(TAG, "Accessory found: " + accessory.getModel());

            // --- ADD THIS PERMISSION CHECK ---
            UsbManager manager = (UsbManager) getSystemService(Context.USB_SERVICE);
            if (manager.hasPermission(accessory)) {
                openPipe(accessory);
            } else {
                Log.d(TAG, "Permission missing in handleIntent. Requesting now.");
                PendingIntent permissionIntent = PendingIntent.getBroadcast(this, 0, new Intent(ACTION_USB_PERMISSION), PendingIntent.FLAG_IMMUTABLE);
                manager.requestPermission(accessory, permissionIntent);
            }
        } else {
            Log.e(TAG, "No accessories found via Intent OR Scan.");
        }
    }
    private void openPipe(UsbAccessory accessory) {
        // 1. EXECUTE ON BACKGROUND THREAD to avoid blocking UI during close/open
        new Thread(() -> {
            synchronized (pipeLock) {
                Log.d(TAG, ">>> Opening Pipe for: " + accessory.getModel());

                // 2. Kill any existing connection with extreme prejudice
                closeAccessoryInternal();

                try {
                    UsbManager manager = (UsbManager) getSystemService(Context.USB_SERVICE);
                    fileDescriptor = manager.openAccessory(accessory);

                    if (fileDescriptor != null) {
                        FileDescriptor fd = fileDescriptor.getFileDescriptor();
                        inputStream = new FileInputStream(fd);
                        isReading = true;
                        isClosing = false;

                        // 3. Start the Reader Thread
                        readThread = new Thread(() -> {
                            Log.i(TAG, "--- Read Thread STARTED ---");
                            byte[] buffer = new byte[16384];

                            // Set thread priority high to ensure we drain buffer fast
                            android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_URGENT_AUDIO);

                            while (isReading) {
                                try {
                                    // BLOCKING READ
                                    int bytesRead = inputStream.read(buffer);

                                    if (bytesRead > 0) {
                                        // Valid Data
                                        byte[] data = Arrays.copyOfRange(buffer, 0, bytesRead);
                                        if (globalEvents != null) {
                                            runOnUiThread(() -> globalEvents.success(data));
                                        }
                                    } else if (bytesRead < 0) {
                                        // -1 means EOF (Cable unplugged or Descriptor died)
                                        Log.e(TAG, "--- Read Thread EOF (-1) ---");
                                        break;
                                    }
                                } catch (IOException e) {
                                    // If we are intentionally closing, this exception is expected.
                                    // Don't log it as an error.
                                    if (!isClosing) {
                                        Log.e(TAG, "--- Read Thread CRASHED: " + e.getMessage());
                                    }
                                    break;
                                }
                            }

                            // Cleanup when thread dies
                            isReading = false;
                            Log.i(TAG, "--- Read Thread STOPPED ---");
                        }, "AOA_Reader_Thread");

                        readThread.start();

                        // Notify UI
                        if (globalEvents != null) {
                            runOnUiThread(() -> globalEvents.success("CONNECTED".getBytes()));
                        }

                    } else {
                        Log.e(TAG, "FATAL: openAccessory returned null (Permission Denied?)");
                    }
                } catch (Exception e) {
                    Log.e(TAG, "Exception in openPipe: " + e.getMessage());
                }
            }
        }).start();
    }

    /* 4. A Synchronized Closer that waits for the thread to die */
    private void closeAccessory() {
        new Thread(() -> {
            synchronized (pipeLock) {
                closeAccessoryInternal();
            }
        }).start();
    }

    private void closeAccessoryInternal() {
        if (isClosing) return; // Already cleanup in progress
        isClosing = true;
        isReading = false;

        Log.d(TAG, "Closing Accessory...");

        try {
            // Closing the stream forces the read() to throw IOException and exit
            if (inputStream != null) inputStream.close();
            if (fileDescriptor != null) fileDescriptor.close();
        } catch (IOException e) {
            // Ignore errors during close
        }

        // Wait for thread to actually finish (prevent zombie threads)
        if (readThread != null && readThread.isAlive()) {
            try {
                // Wait up to 500ms for thread to die gracefully
                readThread.join(500);
            } catch (InterruptedException e) {
                Log.e(TAG, "Interrupted while waiting for read thread to die");
            }
        }

        fileDescriptor = null;
        inputStream = null;
        readThread = null;
        Log.d(TAG, "Accessory Closed & Thread Joined.");
    }

}
