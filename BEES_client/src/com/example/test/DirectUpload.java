package com.example.test;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.net.Socket;

import com.example.test.Config;
import com.example.test.R;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class DirectUpload extends Activity {
	private static final String TAG = "DirectUpload";
	private Button imageUpload = null;
	private EditText imagePath = null;

	private DataOutputStream dataSend = null;
	private Socket socket = null;
	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_directupload);
		
		imageUpload = (Button) findViewById(R.id.btn_direct_upload);
		imagePath = (EditText) findViewById(R.id.imagepath);
		
		imageUpload.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method 
				//uploadImages();
				Runnable uploadRun = new Runnable() {
					@Override
					public void run() {
						File dir = new File(Config.IMAGE_DIR
								+ imagePath.getText().toString() + "/");
						Log.e(TAG, "upload images");
						File[] directoryListing = dir.listFiles();
						
						try {
							Log.e(TAG, Config.HOST);
							socket = new Socket(Config.HOST, Config.PORT);
							dataSend = new DataOutputStream(socket.getOutputStream());
							
							int task_tag = 1;
							int imageNum = directoryListing.length;
							dataSend.writeInt(Config.BigtoLittle32(task_tag));
							dataSend.writeInt(Config.BigtoLittle32(imageNum));
								Log.e("task_tag", Integer.toString(task_tag));
								Log.e("imageNum", Integer.toString(imageNum));
								
								if (directoryListing != null) {

									int length = 0;
									byte[] sendBytes = new byte[1024];
									java.io.FileInputStream fos;
									int picsize = 0;

									try {
										for (File file : directoryListing) {

											fos = new FileInputStream(file.getAbsolutePath());
											picsize = fos.available();
											dataSend.writeInt(Config.BigtoLittle32(picsize));
											while ((length = fos.read(sendBytes, 0,
													sendBytes.length)) > 0) {
												dataSend.write(sendBytes, 0, length);
												dataSend.flush();
												//Log.e("sendLength", Integer.toString(sendLength));
											}	
											
												Log.e("PicSize", Integer.toString(picsize));
												Log.i(TAG, file.getName());
										}
										try {
											Thread.sleep(10);
										} catch (InterruptedException e) {
											// TODO Auto-generated catch block
											e.printStackTrace();
										}
										dataSend.close();
										socket.close();
										Log.e(TAG, "Upload images finished!");
									} catch (IOException e) {
										// TODO Auto-generated catch block
										e.printStackTrace();
									}

								} 

						} catch (IOException ex) {
							Log.e(TAG, "Connection fails!");
							ex.printStackTrace();
							// ShowDialog("login exception" + ex.getMessage());
						}
					}
				};
				Thread thread = new Thread(uploadRun);
				thread.start();
				 AlertDialog.Builder dialog = new AlertDialog.Builder(DirectUpload.this);
				    dialog.setTitle("Direct Upload")
				    .setIcon(android.R.drawable.ic_dialog_info)
				    .setMessage("Upload finished!")
				    .setPositiveButton("OK", new DialogInterface.OnClickListener() {		
					@Override
					public void onClick(DialogInterface dialog, int which) {
						// TODO Auto-generated method stub
						finish();
						Intent intent=new Intent(DirectUpload.this, Main.class);
						startActivity(intent);
					}
				}).create().show();
				try {
					thread.join();
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			
		});
		
	
		    
	}
	
	//public void uploadImages() {
	
	//}
	public boolean onKeyDown(int keyCode, KeyEvent event) {  
	    if (keyCode == KeyEvent.KEYCODE_BACK )  
	    {   
	       finish();
	       Intent intent=new Intent(DirectUpload.this, Main.class);
	       startActivity(intent);
	    }  
	          
	    return false;  
	          
	}
	
	public void showDialog() {
		AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(this);

		// set dialog message
		alertDialogBuilder
				.setTitle("Finish")
				.setMessage("All Images Uploaded!")
				.setCancelable(false)
				.setPositiveButton("Finish",
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int id) {
								// if this button is clicked, close
								// current activity
								finish();
							}
						})
				.setNegativeButton("Continue",
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int id) {
								// if this button is clicked, just close
								// the dialog box and do nothing
								dialog.cancel();
							}
						}).create().show();
	}


}
