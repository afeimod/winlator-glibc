package org.freedesktop.wayland;

import android.annotation.SuppressLint;
import android.graphics.Rect;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.View;

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
                    config.socketPath = getFilesDir().getAbsolutePath() + "/tmp/wayland-0";
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
            mWeston.onTouch(event);
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