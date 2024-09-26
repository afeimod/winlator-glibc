package org.freedesktop.wayland;

import static org.freedesktop.wayland.WlTouch.*;

import android.annotation.SuppressLint;
import android.graphics.Rect;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.SurfaceHolder;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class WestonActivity extends AppCompatActivity {
    private WestonJni mWeston;
    private WestonView westonView;

    @SuppressLint("ClickableViewAccessibility")
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.weston_avtivity_layout);
        westonView = findViewById(R.id.westonView);

        mWeston = new WestonJni();
        mWeston.nativeCreate();

        westonView.getHolder().addCallback(new SurfaceHolder.Callback() {
            private boolean firstCreated = true;
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                mWeston.setSurface(holder.getSurface());
                if (firstCreated) {
                    WestonJni.Config config = mWeston.getConfig();
                    String filesPath = getFilesDir().getAbsolutePath();
                    config.socketPath = filesPath + "/tmp/wayland-0";
                    config.xdgConfigPath = filesPath + "/xdg";
                    config.xdgRuntimePath = filesPath + "/tmp";
                    config.renderRefreshRate = 60;
                    config.rendererType = WestonJni.RendererPixman;
                    config.screenRect = new Rect(0, 0, westonView.getWidth(), westonView.getHeight());
                    config.displayRect = new Rect(0, 0, westonView.getWidth(), westonView.getHeight());
                    config.renderRect = new Rect(0, 0, westonView.getWidth(), westonView.getHeight());
                    mWeston.updateConfig();
                    mWeston.init();
                    mWeston.startDisplay();
                    firstCreated = false;
                }
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                mWeston.setSurface(null);
            }
        });

        westonView.setOnTouchListener((v, event) -> {
            int touchType = switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN->WL_TOUCH_DOWN;
                case MotionEvent.ACTION_MOVE->WL_TOUCH_MOTION;
                case MotionEvent.ACTION_UP->WL_TOUCH_UP;
                case MotionEvent.ACTION_CANCEL->WL_TOUCH_CANCEL;
                default -> -1;
            };

            if (touchType == -1)
                return false;

            mWeston.performTouch(event.getPointerId(event.getActionIndex()), touchType, event.getX(), event.getY());
            return true;
        });
    }

    @Override
    protected void onDestroy() {
        if (mWeston.getNativePtr() != WestonJni.NullPtr) {
            try {
                mWeston.stopDisplay();
            } catch (DisplayException e) {}
            mWeston.nativeDestroy();
        }
        super.onDestroy();
    }
}