package com.example.test;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;

import com.example.test.Config;
import com.example.test.R;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

import com.example.test.DirectUpload;

public class Main extends Activity {

	private Button directUpload = null;
	private Button smartSA = null;
	private EditText ipText = null;
	
	// callback for handling the connection status
	private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
		public void onManagerConnected(int status) {
			switch (status) {
			case LoaderCallbackInterface.SUCCESS: {
				try {
					Log.e("Main", "OpenCV loaded successfully");
					// Not load , Oops
					System.loadLibrary("native-util");
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
			default:
				super.onManagerConnected(status);
			}
		}
	};
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		directUpload = (Button) findViewById(R.id.btn_directupload);
		smartSA = (Button) findViewById(R.id.btn_smartsa);
		ipText = (EditText) findViewById(R.id.ip);
		Config.HOST = ipText.getText().toString().trim();
		ipText.addTextChangedListener(new TextWatcher() {

			@Override
			public void onTextChanged(CharSequence s, int start, int before,
					int count) {
				// TODO Auto-generated method stub
			}

			@Override
			public void beforeTextChanged(CharSequence s, int start, int count,
					int after) {
				// TODO Auto-generated method stub

			}

			@Override
			public void afterTextChanged(Editable s) {
				// TODO Auto-generated method stub

				Config.HOST = ipText.getText().toString().trim();
				Log.e("Main", Config.HOST);
			}
		});
		
		directUpload.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				Intent intent = new Intent(getApplicationContext(),
						DirectUpload.class);
				finish();
				startActivity(intent);
			}
		});
		
		smartSA.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				Intent intent = new Intent(getApplicationContext(),
						SmartSA.class);
				finish();
				startActivity(intent);
			}
		});
	}

	public void onResume() {
		super.onResume();
		// Loads and initializes OpenCV library using OpenCV Engine service.
		// You can choose it.
		OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_11, this,
				mLoaderCallback);
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
}
