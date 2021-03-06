package com.brhsrobosoccer.robonautsbrain;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.ParcelFileDescriptor;
import android.util.Log; 
import android.view.Menu;
import android.widget.TextView; 
import com.android.future.usb.UsbAccessory;
import com.android.future.usb.UsbManager;  
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;


public class MainActivity extends Activity {

	private static final String TAG = MainActivity.class.getSimpleName();
	
	private PendingIntent mPermissionIntent;
	private static final String ACTION_USB_PERMISSION = "com.android.example.USB_PERMISSION";
	private boolean mPermissionRequestPending;  
	
	private UsbManager mUsbManager;
	private UsbAccessory mAccessory;
	private ParcelFileDescriptor mFileDescriptor;
	private FileInputStream mInputStream;
	private FileOutputStream mOutputStream;
	
	private static final byte COMMAND_TEXT = 0xF;
	private static final byte TARGET_DEFAULT = 0xF;
	
	private TextView textView; 
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		mUsbManager = UsbManager.getInstance(this);
		mPermissionIntent = PendingIntent.getBroadcast(this, 0, new Intent(
				ACTION_USB_PERMISSION), 0);
		IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
		filter.addAction(UsbManager.ACTION_USB_ACCESSORY_DETACHED);
		registerReceiver(mUsbReceiver, filter);  
		setContentView(R.layout.activity_main);
		textView = (TextView) findViewById(R.id.textView); 
	}

	/** Called when the activity is resumed from its paused state and immediately after onCreate(). */
	@Override public void onResume() {
		super.onResume();
		if (mInputStream != null && mOutputStream != null) {
			return;
		}  
		UsbAccessory[] accessories = mUsbManager.getAccessoryList();
		UsbAccessory accessory = (accessories == null ? null : accessories[0]);
		if (accessory != null) {
			if (mUsbManager.hasPermission(accessory)) {
				openAccessory(accessory);
				}
			else {
				synchronized (mUsbReceiver) {
					if (!mPermissionRequestPending) {
						mUsbManager.requestPermission(accessory, mPermissionIntent);
						mPermissionRequestPending = true;
						}
					}
				}
			}
		else {
			Log.d(TAG, "mAccessory is null");
		}
	} 
	
	/** Called when the activity is paused by the system. */
	@Override public void onPause() { 
		 super.onPause();
		 closeAccessory();
	 }  
	
	/** Called when the activity is no longer needed prior to being removed from the activity stack. */
	@Override public void onDestroy() {
		super.onDestroy();
		unregisterReceiver(mUsbReceiver);
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

}
