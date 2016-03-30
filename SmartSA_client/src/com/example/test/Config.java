package com.example.test;

import java.io.File;

import android.os.Environment;

//  The configure information
public class Config {
	
	public static String HOST = "";   //The IP address of the server
	public static final int PORT = 13320;			//The port number
	public static final String IMAGE_DIR = getSDPath() + "/imageset/";  //The SD path put the tested image batch

	// Big to little or little to big
	public static int BigtoLittle32(int tag) {
		tag = (((tag & 0xff000000) >> 24) | ((tag & 0x00ff0000) >> 8)
				| ((tag & 0x0000ff00) << 8) | ((tag & 0x000000ff) << 24));
		return tag;
	}

	public static String getSDPath() {
		File sdDir = null;
		boolean sdCardExist = Environment.getExternalStorageState().equals(
				android.os.Environment.MEDIA_MOUNTED); // Determine whether SD card exists

		if (sdCardExist) // 
		{
			sdDir = Environment.getExternalStorageDirectory();// Get the directory
		}
		return sdDir.toString();

	}
}
