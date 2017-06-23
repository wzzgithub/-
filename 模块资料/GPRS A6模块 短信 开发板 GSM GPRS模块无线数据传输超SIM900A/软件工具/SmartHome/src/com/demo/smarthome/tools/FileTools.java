package com.demo.smarthome.tools;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import com.demo.smarthome.service.Cfg;

import android.graphics.Bitmap;

/**
 * 文件工具类
 * 
 * @author Administrator
 * 
 */
public class FileTools {

	/**
	 * 
	 * @param path
	 * @param fileName
	 * @param picBase64Data
	 * @return
	 */
	public static boolean saveFile(String path, String fileName,
			String picBase64Data) {
		boolean ok = false;
		FileOutputStream out = null;
		// path = path.replaceAll(" ", "");
		// fileName = fileName.replaceAll(":", " ");
		fileName = fileName.replaceAll(":", ".");

		File file = new File(path);
		file.mkdirs();// 创建文件夹

		// fileName ="m a-y.jpg";
		System.out.println("path:" + path + "  fileName:" + fileName);

		try {
			out = new FileOutputStream(path + "/" + fileName);
			out.write(picBase64Data.getBytes());
			ok = true;
			// bitmap.compress(Bitmap.CompressFormat.JPEG, 100, b);// 把数据写入文件
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally {
			try {
				out.flush();
				out.close();
			} catch (IOException e) {
				e.printStackTrace();
			}

		}
		return ok;
	}

	public static boolean saveFile(String path, String fileName, Bitmap bitmap) {
		boolean ok = false;
		FileOutputStream out = null;

		// path = path.replaceAll(" ", "");
		fileName = fileName.replaceAll(":", ".");
		File file = new File(path);
		file.mkdirs();// 创建文件夹

		try {
			out = new FileOutputStream(path + "/" + fileName);
			// out.write( picBase64Data.getBytes() );
			// ok = true;
			bitmap.compress(Bitmap.CompressFormat.JPEG, 100, out);// 把数据写入文件
			// out.flush();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally {
			try {
				out.flush();
				out.close();
			} catch (IOException e) {
				e.printStackTrace();
			}

		}
		return ok;
	}

	/**
	 * 
	 * @param path
	 * @param fileName
	 * @return
	 */
	public static String getFile(String path, String fileName) {
		String result = "";
		// System.out.println("path:"+path+"  fileName:"+fileName);

		// path = path.replaceAll(" ", "");
		fileName = fileName.replaceAll(":", ".");
		File file = new File(path + "/" + fileName);
		int len = (int) file.length();
		FileInputStream in = null;
		byte[] temp = new byte[len];
		try {
			in = new FileInputStream(path + "/" + fileName);
			// System.out.println("len:"+len);
			in.read(temp, 0, len);
			// System.out.println("temp:"+temp);
			result = new String(temp, "UTF-8");
			// System.out.println("result:"+result);
			in.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally {
			try {
				if (in != null) {
					in.close();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}

		}
		return result;
	}

	public static byte[] getFileToByte(String path, String fileName) {
		// System.out.println("path:"+path+"  fileName:"+fileName);

		// path = path.replaceAll(" ", "");
		fileName = fileName.replaceAll(":", ".");
		File file = new File(path + "/" + fileName);
		int len = (int) file.length();
		FileInputStream in = null;
		byte[] temp = new byte[len];
		try {
			in = new FileInputStream(path + "/" + fileName);
			// System.out.println("len:"+len);
			in.read(temp, 0, len);
			in.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally {
			try {
				if (in != null) {
					in.close();
				}
			} catch (IOException e) {
				e.printStackTrace();
			}

		}
		return temp;
	}

	public static boolean deleteFile(String path, String fileName) {
		boolean ok = false;

		// path = path.replaceAll(" ", "");
		fileName = fileName.replaceAll(":", ".");
		File file = new File(Cfg.savePath + "/" + fileName);
		try {
			if (file.isFile() && file.exists()) {

				if (file.delete()) {
					ok = true;
				}
			}
		} catch (Exception e) {

		}
		return ok;
	}
}
