package com.winlator.renderer;

import android.view.LayoutInflater;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.SeekBar;

import com.winlator.R;
import com.winlator.XServerDisplayActivity;
import com.winlator.contentdialog.ContentDialog;

public class ScreenFxDialog extends ContentDialog {
    private final XServerDisplayActivity activity;
    private final LayoutInflater inflater;

    public ScreenFxDialog(XServerDisplayActivity activity) {
        super(activity, R.layout.screen_effect_dialog);
        this.activity = activity;
        setCancelable(false);
        setTitle(R.string.screen_effect);
        setIcon(R.drawable.icon_task_manager);

        inflater = LayoutInflater.from(activity);

        Button BTCancel = findViewById(R.id.BTCancel);
        BTCancel.setText(R.string.reset);

        CheckBox CBAntialiasing = findViewById(R.id.CBAntialiasing);

        SeekBar SBBrightness = findViewById(R.id.SBBrightness);
        SeekBar SBContrast = findViewById(R.id.SBContrast);
        SeekBar SBGamma = findViewById(R.id.SBGamma);
        SeekBar SBReflection = findViewById(R.id.SBReflection);
        SeekBar SBSaturation = findViewById(R.id.SBSaturation);
    }
}
