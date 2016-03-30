package com.example.test;

public class NativeUtil {
	
	public native static void computeDescripors(long mGrayAddr, long mRgbaAddr, long mOutputAddr);
	
	public native static void detectFeatures(long mGrayAddr, long mRgbaAddr, long mOutputAddr);
	
	public native static int[] transformToGray(int[] pixels, int w, int h) ;
	/*
	 * A native method that is implemented by the 'hello-jni' native library,
	 * which is packaged with this application.
	 */
	public native static String stringFromJNI();

	/*
	 * This is another native method declaration that is *not* implemented by
	 * 'hello-jni'. This is simply to show that you can declare as many native
	 * methods in your Java code as you want, their implementation is searched
	 * in the currently loaded native libraries only the first time you call
	 * them.
	 * 
	 * Trying to call this function will result in a
	 * java.lang.UnsatisfiedLinkError exception !
	 */
	public native String unimplementedStringFromJNI();

}
