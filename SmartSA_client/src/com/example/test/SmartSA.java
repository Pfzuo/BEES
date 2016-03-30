package com.example.test;

import com.example.test.Config;
import com.example.test.ORB;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.Charset;
import java.util.Arrays;

import org.opencv.core.Mat;
import org.opencv.core.Size;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class SmartSA extends Activity {
	private static final String TAG = "SmartSA";
	private Button imageUpload = null;
	private EditText imagePath = null;
	
	private Socket socket = null;
	private DataOutputStream dataSend = null;
	private DataInputStream dataReceive = null;

	private static int battery = 100;
	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_smartsa);
		
		imageUpload = (Button) findViewById(R.id.btn_smartsa_upload);
		imagePath = (EditText) findViewById(R.id.smartsa_imagepath);
		registerReceiver(mBatteryInfoReceiver, new IntentFilter(
				Intent.ACTION_BATTERY_CHANGED));
		imageUpload.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				Log.e("battery************", Integer.toString(battery));
				uploadImages(battery);
				
				// Extract ORB
				// Upload ORB
				// Receive results
				// Upload images
			}
		});
		
	}
	
	//	Get the remaining energy of the smartphone battery
	BroadcastReceiver mBatteryInfoReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			if (Intent.ACTION_BATTERY_CHANGED.equals(action)) {

				int level = intent.getIntExtra("level", 0);
				int scale = intent.getIntExtra("scale", 100);
				battery = level * 100 / scale;
				Log.e("battery1", Integer.toString(battery));
			}
		}
	};
	
	//Upload images using SmartSA
	public void uploadImages(final int remEnergy) {
		
		Runnable uploadRun = new Runnable() {
			@Override
			public void run() {
				
				//	The resolution compression proportion for energy-aware feature extraction
				float extractCompressRatio = (float)(0.4-0.4*remEnergy/100);
				
				//	The resolution compression proportion for energy-aware image upload
				float uploadCompressRatio = (float)(0.8-0.8*remEnergy/100);
				
				File dir = new File(Config.IMAGE_DIR + imagePath.getText().toString() + "/");
				File[] directoryListing = dir.listFiles();
				/*if(!dir.isDirectory() || directoryListing.length == 0){
				    Intent intent=new Intent(SmartSA.this, Main.class);
				    finish();
				    startActivity(intent);
				}*/
				
				Bitmap newBitmap = null;
				Mat ORBMat = null;
				
				try {
					socket = new Socket(Config.HOST, Config.PORT);
					dataSend = new DataOutputStream(socket.getOutputStream());
					dataReceive = new DataInputStream(socket.getInputStream());
					
					int taskTag = 2;
					int imageNum = directoryListing.length;
					
					dataSend.writeInt(Config.BigtoLittle32(taskTag));
					dataSend.writeInt(Config.BigtoLittle32(remEnergy));
					dataSend.writeInt(Config.BigtoLittle32(imageNum));
					
						Log.e("task_tag", Integer.toString(taskTag));
						Log.e("battery", Integer.toString(remEnergy));
						Log.e("imageNum", Integer.toString(imageNum));
						
						int length = 0;
						int matbytes = 0;

						try {
							for (File file : directoryListing) {
								Bitmap bitmap = BitmapFactory.decodeFile(file
										.getAbsolutePath());
								Matrix matrix = new Matrix();
								matrix.postScale(extractCompressRatio, extractCompressRatio);
								newBitmap = Bitmap.createBitmap(bitmap, 0,
										0, bitmap.getWidth(), bitmap.getHeight(), matrix, true);

								ORBMat = ORB.extractImageORBDescriptors(newBitmap);										//Log.e(TAG, "Width:" + resultBitmap.getWidth() + ", Height:" + resultBitmap.getHeight());
								ORBMat.dump();
								//	Log.e("dump", ORBMat.dump());

								matbytes = (int) (ORBMat.width() * ORBMat.height());
								Log.e("matbytes", Long.toString(matbytes));

								int col = ORBMat.cols();
								int row = ORBMat.rows();
								Log.e("col", Integer.toString(col));
								Log.e("row", Integer.toString(row));
							   
								Size ORBMatSize = ORBMat.size();
								byte[] bytes = new byte[(int) (ORBMat.width() * ORBMat.height())];
								ORBMat.get(0, 0, bytes);			//Get the byte array of ORB Mat
								//Log.e("BYTES", Arrays.toString(bytes));

								dataSend.writeInt(Config.BigtoLittle32(bytes.length));
								Log.e("bytes.length", Integer.toString(bytes.length));
								dataSend.write(bytes);
								Log.e("sb",  Arrays.toString(bytes));
								/*int z = 0;   
						        StringBuffer sb = new StringBuffer();
								for (int x = 0; x < (int)ORBMatSize.height; x++){
									for (int y = 0; y < (int)ORBMatSize.width; y++){
										sb.append(bytes[(int)(x * ORBMatSize.width + y)]);
								        z ++;
								        sb.append(" ");
								        if(z == 32){
								              sb.append("\n");
								              z = 0;
								        }
								     }
								}
								dataSend.writeInt(Config.BigtoLittle32(sb.toString().getBytes("utf-8").length));
								Log.e("bytes.length", Integer.toString(sb.toString().getBytes("utf-8").length));
								dataSend.write(sb.toString().getBytes("utf-8"));						
							    Log.e("sb",  Arrays.toString(sb.toString().getBytes("utf-8")));*/
							    
								dataSend.flush();
								Log.e(TAG, file.getName());
								Log.e(TAG, "" + length);
							}
								Log.e(TAG, "Upload ORB finishes!");
								
						} catch (IOException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}

						
						Log.e(TAG, "Receive feedback!");
						int[] feedback = new int[imageNum];
	                    int len = 0; 
	                    int sendImagesNum = 0;
                    	while (len < imageNum) { 
                    		feedback[len] = Config.BigtoLittle32(dataReceive.readInt());
                    		if(feedback[len] == 1)
                    			sendImagesNum ++;
                    		len ++;
                    	}
                    	 Log.e("feedback", Arrays.toString(feedback));
                    	 Log.e(TAG, "Receive feedback over!");

                    	java.io.FileInputStream fos;
						dataSend.writeInt(Config.BigtoLittle32(sendImagesNum));
						Log.e("sendImagesNum", Integer.toString(sendImagesNum));
                    	
                    	int i = 0;
						for (File file1 : directoryListing) {
                    		if(feedback[i] == 1){
								fos = new FileInputStream(file1.getAbsolutePath());
								//picsize = fos.available();
								//dataSend.writeInt(Config.BigtoLittle32(picsize));
								Bitmap sendBitmap = BitmapFactory.decodeFile(file1.getAbsolutePath());
								
								Matrix sendMatrix = new Matrix();
								sendMatrix.postScale(uploadCompressRatio, uploadCompressRatio);
								Bitmap newSendBitmap = Bitmap.createBitmap(sendBitmap, 0,
										0, sendBitmap.getWidth(), sendBitmap.getHeight(), sendMatrix, true);
								
								ByteArrayOutputStream baos = new ByteArrayOutputStream();
								newSendBitmap.compress(Bitmap.CompressFormat.JPEG, 85, baos);
								byte[] sendImages  = baos.toByteArray();

								dataSend.writeInt(Config.BigtoLittle32(sendImages.length));
								dataSend.write(sendImages);
								dataSend.flush();
								
								Log.e("sendImages.length", Integer.toString(sendImages.length));
								Log.i(TAG, file1.getName());
								
								//sendImages = new byte[picsize];
								//while ((length = fos.read(sendImages, 0,
									//	sendImages.length)) > 0) {
								//	dataSend.write(sendImages, 0, length);
								//	dataSend.flush();
								//}								
                    		 }
                    		 i ++;
						}
						
                    	dataReceive.close();
                    	dataSend.close();
                    	socket.close();
                    	 
				} catch (IOException ex) {
					ex.printStackTrace();
					// ShowDialog("login exception" + ex.getMessage());
				}
			}
		};
		Thread thread = new Thread(uploadRun);
		thread.start();
		try {
			thread.join();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		AlertDialog.Builder dialog = new AlertDialog.Builder(SmartSA.this);
		    dialog.setTitle("SmartSA")
		    .setIcon(android.R.drawable.ic_dialog_info)
		    .setMessage("Upload successfully!")
		    .setPositiveButton("Sure", new DialogInterface.OnClickListener() {		
			@Override
			public void onClick(DialogInterface dialog, int which) {
				// TODO Auto-generated method stub
				finish();
				Intent intent=new Intent(SmartSA.this, Main.class);
				startActivity(intent);
			}
		}).create().show();
	}
	
	
	public static String getSDPath(){ 
	       File sdDir = null; 
	       boolean sdCardExist = Environment.getExternalStorageState()   
	                           .equals(android.os.Environment.MEDIA_MOUNTED);    

	       if (sdCardExist)     
	       {                               
	         sdDir = Environment.getExternalStorageDirectory();
	      }   
	       return sdDir.toString(); 
	       
	}
	
	public boolean onKeyDown(int keyCode, KeyEvent event) {  
	    if (keyCode == KeyEvent.KEYCODE_BACK )  
	    {   
	       finish();
	       Intent intent=new Intent(SmartSA.this, Main.class);
	       startActivity(intent);
	    }  
	         
	    return false;  
	          
	}
}



