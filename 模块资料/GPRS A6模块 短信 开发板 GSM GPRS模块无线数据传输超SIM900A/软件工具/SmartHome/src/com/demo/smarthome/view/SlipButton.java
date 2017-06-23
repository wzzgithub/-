package com.demo.smarthome.view;

import com.demo.smarthome.R;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;

public class SlipButton extends View implements OnTouchListener {
	// 当前按钮状态
	private boolean nowChoose = false;
	// 用户是否在滑动
	private boolean onSlip = false;
	// 按下时的X，当时的X
	private float downX, nowX;
	// 打开和关闭状态下的，游标的Rect
	private Rect btn_On, btn_Off;

	private boolean isChgLsnOn = false;
	private com.demo.smarthome.view.OnChangedListener ChgLsn;

	private Bitmap bg_on, bg_off, slip_btn;

	public SlipButton(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
		init();
	}

	public SlipButton(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
		init();
	}

	public SlipButton(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		// TODO Auto-generated constructor stub
		init();
	}

	private void init() {
		// TODO Auto-generated method stub
		bg_on = BitmapFactory.decodeResource(getResources(), R.drawable.on);// R.drawable.btnopen
		bg_off = BitmapFactory.decodeResource(getResources(), R.drawable.off);// R.drawable.btnclose
		slip_btn = BitmapFactory.decodeResource(getResources(),
				R.drawable.sb_bg);
		int tmp = bg_off.getWidth() / 2;
		btn_On = new Rect(tmp, 0, slip_btn.getWidth() + tmp,
				slip_btn.getHeight());
		btn_Off = new Rect(bg_off.getWidth() - tmp - slip_btn.getWidth(), 0,
				bg_off.getWidth() - tmp, slip_btn.getHeight());
		setOnTouchListener(this);
	}

	@Override
	protected void onDraw(Canvas canvas) {
		// TODO Auto-generated method stub
		super.onDraw(canvas);
		Matrix matrix = new Matrix();
		Paint paint = new Paint();
		float x;
		if (onSlip) {
			if (nowX >= bg_on.getWidth()) {
				x = bg_on.getWidth() - slip_btn.getWidth() / 2;
			} else {
				x = nowX - slip_btn.getWidth() / 2;
			}
		} else {
			if (nowChoose) {
				x = btn_On.left;
			} else {
				x = btn_Off.left;
			}
		}
		if (nowX < (bg_on.getWidth() / 2)) {
			canvas.drawBitmap(bg_off, matrix, paint);
		} else {
			canvas.drawBitmap(bg_on, matrix, paint);
		}
		if (x < 0) {
			x = 0;
		} else if (x > bg_on.getWidth() - slip_btn.getWidth()) {
			x = bg_on.getWidth() - slip_btn.getWidth();
		}
		canvas.drawBitmap(slip_btn, x, 0, paint);
	}

	public boolean onTouch(View v, MotionEvent event) {
		// TODO Auto-generated method stub
		switch (event.getAction()) {
		case MotionEvent.ACTION_MOVE:
			nowX = event.getX();
			break;
		case MotionEvent.ACTION_DOWN:
			if (event.getX() > bg_on.getWidth()
					|| event.getY() > bg_on.getHeight()) {
				return false;
			}
			onSlip = true;
			downX = event.getX();
			nowX = downX;
			break;
		case MotionEvent.ACTION_UP:
			onSlip = false;
			boolean lastChoose = nowChoose;
			if (event.getX() >= (bg_on.getWidth() / 2)) {
				nowChoose = true;
			} else {
				nowChoose = false;
			}
			if (isChgLsnOn && (lastChoose != nowChoose)) {
				ChgLsn.OnChanged(nowChoose);
			}
			break;
		default:
			break;
		}
		invalidate();
		return true;
	}

	public void setOnChangeListener(
			com.demo.smarthome.view.OnChangedListener onChangedListener) {
		isChgLsnOn = true;
		ChgLsn = onChangedListener;
	}

	public boolean isNowChoose() {
		return nowChoose;
	}

	public void setNowChoose(boolean nowChoose) {
		this.nowChoose = nowChoose;
		nowX = btn_On.left;
		invalidate();
	}

}
