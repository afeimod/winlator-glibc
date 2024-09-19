package org.freedesktop.wayland;

import android.os.Bundle;
import android.view.SurfaceHolder;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class WestonActivity extends AppCompatActivity {
    private WestonJni mWeston;
    private WestonView westonView;

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
                mWeston.setRenderSurface(holder.getSurface());
                if (firstCreated) {
                    mWeston.setScreenSize(1280, 720);
                    mWeston.setRefreshRate(60);
                    mWeston.setRenderer(WestonJni.RendererPixman);
                    if (!mWeston.initWeston()) {
                        throw new RuntimeException("Failed to init weston");
                    }
                    mWeston.startDisplay();
                    firstCreated = false;
                }
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                mWeston.setRenderSurface(null);
            }
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