package com.demo.smarthome.activity;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.demo.smarthome.R;
import com.demo.smarthome.dao.DevDao;
import com.demo.smarthome.device.Dev;
import com.demo.smarthome.iprotocol.IProtocol;
import com.demo.smarthome.protocol.MSGCMD;
import com.demo.smarthome.protocol.MSGCMDTYPE;
import com.demo.smarthome.protocol.Msg;
import com.demo.smarthome.protocol.PlProtocol;
import com.demo.smarthome.service.Cfg;
import com.demo.smarthome.service.HttpConnectService;
import com.demo.smarthome.service.SocketService;
import com.demo.smarthome.service.SocketService.SocketBinder;
import com.demo.smarthome.tools.StrTools;
import com.demo.smarthome.zxing.demo.CaptureActivity;

import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;

/**
 * 主界面类
 * 
 * @author Administrator
 * 
 */
public class MainActivity extends Activity {

	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();

		new GetDevListThread().start();
	}

	Button btnRefresh = null;
	Button btnAddDev = null;
	ListView listView;
	private final String TAG = "MainActivity";
	IProtocol protocol = new PlProtocol();
	Msg msg = new Msg();
	static final int GET_DEV_SUCCEED = 0;
	static final int GET_DEV_ERROR = 1;
	static final int BUTTON_DELETE = 2;
	static final int BUTTON_CONTROL = 3;
	static final int DELETE_SUCCEED = 4;
	static final int DELETE_ERROR = 5;

	Handler handler = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			// TODO Auto-generated method stub
			super.handleMessage(msg);
			// dataToui();
			switch (msg.what) {

			case GET_DEV_SUCCEED:
				Toast.makeText(MainActivity.this, "获取设备列表成功！",
						Toast.LENGTH_SHORT).show();
				changeDevList();

				break;
			case GET_DEV_ERROR:
				// Toast.makeText(MainActivity.this, "获取设备列表成功！",
				// Toast.LENGTH_SHORT).show();
				Cfg.listDev.clear();
				changeDevList();

				break;

			case BUTTON_DELETE:
				// Toast.makeText(MainActivity.this, "BUTTON_DELETE:" +
				// msg.arg1,
				// Toast.LENGTH_SHORT).show();
				// Toast.makeText(MainActivity.this,
				// "BUTTON_CONTROL:"+msg.arg1,Toast.LENGTH_SHORT).show();
				HashMap<String, Object> data1 = (HashMap<String, Object>) listView
						.getItemAtPosition(msg.arg1);
				String devId1 = (String) data1.get("id");

				Log.i(TAG, "ItemClickListener devId：" + devId1);
				Toast.makeText(getApplicationContext(), "选择设备" + devId1, 0)
						.show();
				Dev dev1 = getDevById(devId1);
				if (dev1 == null) {
					Toast.makeText(getApplicationContext(), "请重新选择设备", 0)
							.show();
					return;
				}
				new DeleteDevThread(dev1.getId()).start();
				break;
			case BUTTON_CONTROL:
				// Toast.makeText(MainActivity.this,
				// "BUTTON_CONTROL:"+msg.arg1,Toast.LENGTH_SHORT).show();
				HashMap<String, Object> data = (HashMap<String, Object>) listView
						.getItemAtPosition(msg.arg1);
				String devId = (String) data.get("id");

				Log.i(TAG, "ItemClickListener devId：" + devId);
				Toast.makeText(getApplicationContext(), "选择设备" + devId, 0)
						.show();
				Dev dev = getDevById(devId);
				if (dev == null) {
					Toast.makeText(getApplicationContext(), "请重新选择设备", 0)
							.show();
					return;
				}
				// 跳转到设置界面
				Intent intent = new Intent();
				intent.setClass(MainActivity.this, DevViewActivity.class);

				Bundle bundle = new Bundle();
				bundle.putString("devId", dev.getId());
				intent.putExtras(bundle);

				Log.i(TAG, "ItemClickListener dev：" + dev.getId());
				// MyLog.i(TAG, "跳转至设置界面");DeleteDevThread
				startActivity(intent);// 打开新界面
				break;

			case DELETE_SUCCEED:
				Toast.makeText(MainActivity.this, "删除设备成功。", Toast.LENGTH_SHORT)
						.show();
				new GetDevListThread().start();
				break;
			case DELETE_ERROR:
				Toast.makeText(MainActivity.this, "删除设备失败！", Toast.LENGTH_SHORT)
						.show();

				break;
			default:
				break;

			}
		}

	};

	SocketBinder socketBinder;
	SocketService socketService;
	boolean isBinderConnected = false;

	IntentFilter intentFilter = null;
	SocketIsConnectReceiver socketConnectReceiver = new SocketIsConnectReceiver();

	private class SocketIsConnectReceiver extends BroadcastReceiver {// 继承自BroadcastReceiver的子类
		@Override
		public void onReceive(Context context, Intent intent) {// 重写onReceive方法

			if (intent.getBooleanExtra("conn", false)) {
				Log.i(TAG, "socket连接成功。");
			} else {
				Log.i(TAG, "socket连接失败。");
			}
		}
	}

	private ServiceConnection conn = new ServiceConnection() {

		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			// TODO Auto-generated method stub
			Log.i(TAG, "=============onServiceConnected");
			socketBinder = (SocketBinder) service;
			socketService = socketBinder.getService();
			socketService.myMethod();

			isBinderConnected = true;
		}

		@Override
		public void onServiceDisconnected(ComponentName name) {
			// TODO Auto-generated method stub
			Log.i(TAG, "xxxxxxxxxxxxxxxxxxxxxxxxxxxonServiceDisconnected");
			isBinderConnected = false;
			socketBinder = null;
			socketService = null;
		}

	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE); // 注意顺序
		setContentView(R.layout.activity_main);

		TextView title = (TextView) findViewById(R.id.titleMain);
		title.setClickable(true);
		title.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				finish();
			}

		});

		btnRefresh = (Button) findViewById(R.id.setupBtnRefresh);
		btnRefresh.setOnClickListener(new BtnRefreshOnClickListener());

		btnAddDev = (Button) findViewById(R.id.mainBtnAddDev);
		btnAddDev.setOnClickListener(new BtnAddDevOnClickListener());

		listView = (ListView) this.findViewById(R.id.devListView);
		// 条目点击事件
	//	listView.setOnItemClickListener(new ItemClickListener());

		// Cfg.listDev =new
		// DevDao(MainActivity.this.getBaseContext()).getDevList();
		new GetDevListThread().start();

		changeDevList();

		// bind....
		try {
			if (!isBinderConnected) {
				bindService();
			}
		} catch (Exception e) {

		}
		/*
		 *  */
		intentFilter = new IntentFilter();
		intentFilter.addAction(Cfg.SendBoardCastName);
		this.registerReceiver(socketConnectReceiver, intentFilter);
	}

	private void changeDevList() {
		List<HashMap<String, Object>> data = new ArrayList<HashMap<String, Object>>();
		for (Dev dev : Cfg.listDev) {
			HashMap<String, Object> item = new HashMap<String, Object>();
			item.put("id", dev.getId());
			item.put("name", dev.getNickName());
			item.put("state", dev.isOnLine() ? "在线" : "不在线");
			data.add(item);
		}
		// 创建SimpleAdapter适配器将数据绑定到item显示控件上
		SimpleAdapter adapter = new MySimpleAdapter(this, data,
				R.layout.devitem, new String[] { "id", "name", "state" },
				new int[] { R.id.devId, R.id.devName, R.id.devStat });
		// 实现列表的显示
		listView.setAdapter(adapter);
		// 删除分割线
		listView.setDivider(null);
	}

	private void bindService() {
		Intent intent = new Intent(MainActivity.this, SocketService.class);
		bindService(intent, conn, Context.BIND_AUTO_CREATE);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	/**
	 * 获得设备列表线程
	 * 
	 * @author Administrator
	 * 
	 */
	class GetDevListThread extends Thread {

		@Override
		public void run() {
			// Cfg.listDev =new
			// DevDao(MainActivity.this.getBaseContext()).getDevList();
			// changeDevList();
			Message message = new Message();
			message.what = GET_DEV_ERROR;

			Log.v("GetDevListThread", "GetDevListThread start..");

			List<Dev> listDev = HttpConnectService.getDeviceList(Cfg.userName,
					new String(Cfg.torken));

			for (Dev dev : listDev) {
				Log.v("GetDevListThread", "dev:" + dev);

			}
			if (listDev.size() > 0) {
				Cfg.listDev = listDev;
				message.what = GET_DEV_SUCCEED;
			}
			handler.sendMessage(message);
		}
	}

	/**
	 * 刷新 按钮监听事件
	 * 
	 * @author Administrator
	 * 
	 */
	class BtnRefreshOnClickListener implements OnClickListener {
		@Override
		public void onClick(View v) {
			new GetDevListThread().start();
		}
	}

	class BtnAddDevOnClickListener implements OnClickListener {
		@Override
		public void onClick(View v) {
			Intent intent = new Intent();
			intent.setClass(MainActivity.this, ScanActivity.class);
			startActivity(intent);// 打开新界面
		}
	}

	// 获取点击事件
	private final class ItemClickListener implements OnItemClickListener {

		public void onItemClick(AdapterView<?> parent, View view, int position,
				long id) {
			ListView listView = (ListView) parent;
			HashMap<String, Object> data = (HashMap<String, Object>) listView
					.getItemAtPosition(position);
			String devId = (String) data.get("id");

			Log.i(TAG, "ItemClickListener devId：" + devId);
			Toast.makeText(getApplicationContext(), "选择设备" + devId, 0).show();
			Dev dev = getDevById(devId);
			if (dev == null) {
				Toast.makeText(getApplicationContext(), "请重新选择设备", 0).show();
				return;
			}
			// 跳转到设置界面
			Intent intent = new Intent();
			intent.setClass(MainActivity.this, DevViewActivity.class);

			Bundle bundle = new Bundle();
			bundle.putString("devId", dev.getId());
			intent.putExtras(bundle);

			Log.i(TAG, "ItemClickListener dev：" + dev.getId());
			// MyLog.i(TAG, "跳转至设置界面");
			startActivity(intent);// 打开新界面

		}
	}

	/**
	 * 通过设备id获取设备对象
	 * 
	 * @param id
	 *            设备id
	 * @return 设备对象
	 */
	private Dev getDevById(String id) {
		if (id == null) {
			return null;
		}
		for (Dev dev : Cfg.listDev) {
			if (dev.getId().equals(id)) {
				return dev;
			}
		}
		return null;
	}

	class MySimpleAdapter extends SimpleAdapter {

		// protected static final int BUTTON_DELETE = 0;
		// protected static final int BUTTON_ADD = 0;
		public MySimpleAdapter(Context context,
				List<? extends Map<String, ?>> data, int resource,
				String[] from, int[] to) {
			super(context, data, resource, from, to);
			// TODO Auto-generated constructor stub
		}

		public View getView(int position, View convertView, ViewGroup parent) {
			// TODO Auto-generated method stub
			final int mPosition = position;
			convertView = super.getView(position, convertView, parent);
			ImageView buttonAdd = (ImageView) convertView
					.findViewById(R.id.devControl);// id为你自定义布局中按钮的id
			buttonAdd.setOnClickListener(new View.OnClickListener() {

				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					// mHandler.obtainMessage(BUTTON_ADD, mPosition, 0)
					// .sendToTarget();

					Message message = new Message();
					message.what = BUTTON_CONTROL;
					message.arg1 = mPosition;
					handler.sendMessage(message);

				}
			});
			ImageView buttonDelete = (ImageView) convertView
					.findViewById(R.id.devDelete);
			buttonDelete.setOnClickListener(new View.OnClickListener() {

				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					// mHandler.obtainMessage(BUTTON_DELETE, mPosition, 0)
					// .sendToTarget();

					Message message = new Message();
					message.what = BUTTON_DELETE;
					message.arg1 = mPosition;
					handler.sendMessage(message);
				}
			});
			return convertView;
		}
		// private Handler mHandler = new Handler() {
		//
		// @Override
		// public void handleMessage(Message msg) {
		// // TODO Auto-generated method stub
		// super.handleMessage(msg);
		// switch (msg.what) {
		// case BUTTON_ADD:
		// HashMap<String, Object> map = new HashMap<String, Object>();
		// mList.add(map);
		// notifyDataSetChanged();// 这个函数很重要，告诉Listview适配器数据发生了变化
		// mShowInfo.setText("你点击了第" + (msg.arg1 + 1) + "按钮");
		// break;
		// case BUTTON_DELETE:
		// mList.remove(msg.arg1);
		// notifyDataSetChanged();
		// mShowInfo.setText("你删除了第" + (msg.arg1 + 1) + "行");
		// break;
		// }
		// }
		//
		// };

	}

	class DeleteDevThread extends Thread {
		String id;
		int delay = 50;

		public DeleteDevThread(String strId) {
			id = strId;

		}

		@Override
		public void run() {
			// try {
			// Thread.sleep(1000);
			// } catch (InterruptedException e1) {
			// // TODO Auto-generated catch block
			// e1.printStackTrace();
			// }
			if (id.isEmpty()) {
				return;
			}

			Message message = new Message();
			message.what = DELETE_ERROR;

			Log.v("SendLigheStateThread", "开始提交数据 start..");
			// int delay = 50;
			int reSendTime = 1;
			// Cfg.isSubmitDev = false;

			String tmp;
			if (StrTools.stringToInt(id.substring(0, 1)) == 0) {
				tmp = id.substring(1);
			} else {
				tmp = id;
			}

			byte[] data = new byte[8];
			int index = 0;
			long val = 0;

			// id 低位在前
			val = StrTools.stringToInt(tmp);
			for (int i = 0; i < 8; i++) {
				data[index++] = (byte) (val % 256);
				val /= 256;
			}
			// // pass 高位在前
			// byte[] b = new byte[8];
			// val = StrTools.stringToInt(pass);
			// for (int i = 0; i < 8; i++) {
			// b[i] = (byte) (val % 256);
			// val /= 256;
			// }
			// byte[] buff = StrTools.byteToSwapByte(b);
			// for (int i = 0; i < 8; i++) {
			// // if(buff[i] != 0){
			// data[index++] = buff[i];
			// // }
			// }
			msg.setId(Cfg.userId);
			msg.setCmdType(MSGCMDTYPE.valueOf((byte) 0xEF));
			msg.setCmd(MSGCMD.valueOf((byte) 0x07));
			msg.setTorken(Cfg.tcpTorken);
			msg.setData(data);
			msg.setDataLen(data.length);
			protocol.MessageEnCode(msg);

			Log.i(TAG, "         data:" + StrTools.bytesToHexString(data));
			Log.i(TAG,
					"sendData:" + StrTools.bytesToHexString(msg.getSendData()));
			Cfg.isDeleteDev = false;
			while (true) {

				try {
					Thread.sleep(100);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				reSendTime--;
				if (reSendTime <= 0) {// send
					reSendTime = 10;

					socketService.socketSendMessage(msg);//

				}
				// 查询
				if (Cfg.isDeleteDev) {
					message.what = DELETE_SUCCEED;
					handler.sendMessage(message);
					break;
				}

				delay--;
				if (delay <= 0) {
					message.what = DELETE_ERROR;
					handler.sendMessage(message);
					break;
				}
			}

		}
	}
}
