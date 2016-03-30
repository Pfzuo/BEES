package com.example.test;

import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;

import android.graphics.Bitmap;


import com.example.test.NativeUtil;


public class ORB {
	private static final String TAG = "Extract ORB";
	public static Mat extractImageORBDescriptors(Bitmap bitmap) {
		int width = bitmap.getWidth();
		int height = bitmap.getHeight();
		Mat mRgba = new Mat(height, width, CvType.CV_8U, new Scalar(4));
		Mat mGray = new Mat(height, width, CvType.CV_8U, new Scalar(4));
		Mat output = new Mat();

		Utils.bitmapToMat(bitmap, mRgba);

		// Converts an image from one color space to another.
		Imgproc.cvtColor(mRgba, mGray, Imgproc.COLOR_RGB2GRAY, 4);

		NativeUtil.computeDescripors(mGray.getNativeObjAddr(),
				mRgba.getNativeObjAddr(), output.getNativeObjAddr());
		
		return output;
		// Then convert the processed Mat to Bitmap
		//Bitmap resultBitmap = Bitmap.createBitmap(output.cols(), output.rows(), Bitmap.Config.ARGB_8888);
			//Log.e(TAG, "Width:" + resultBitmap.getWidth() + ", Height:" + resultBitmap.getHeight());

		//Utils.matToBitmap(output, resultBitmap);

		//return resultBitmap;
	}
}