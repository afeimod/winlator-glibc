package com.winlator.renderer;

import android.annotation.SuppressLint;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.SeekBar;

import com.winlator.R;
import com.winlator.XServerDisplayActivity;
import com.winlator.contentdialog.ContentDialog;

public class ScreenFxDialog extends ContentDialog implements SeekBar.OnSeekBarChangeListener {
    private final ScreenFxModel model = ScreenFxModel.getInstance();

    public ScreenFxDialog(XServerDisplayActivity activity) {
        super(activity, R.layout.screen_effect_dialog);
        setCancelable(false);
        setTitle(R.string.screen_effect);
        setIcon(R.drawable.icon_task_manager);
        model.load(getContext());

        Button BTCancel = findViewById(R.id.BTCancel);
        BTCancel.setText(R.string.reset);
        BTCancel.setOnClickListener(view -> {
            model.reset(getContext());
            dismiss();
        });

        Button BTConfirm = findViewById(R.id.BTConfirm);
        BTConfirm.setOnClickListener(view -> {
            model.save(getContext());
            dismiss();
        });

        CheckBox CBAntialiasing = findViewById(R.id.CBAntialiasing);
        CBAntialiasing.setChecked(model.antialiasing);
        CBAntialiasing.setOnCheckedChangeListener((compoundButton, value) -> model.antialiasing = value);

        SeekBar SBBrightness = findViewById(R.id.SBBrightness);
        SBBrightness.setProgress((int) (model.brightness * 100));
        SBBrightness.setOnSeekBarChangeListener(this);

        SeekBar SBContrast = findViewById(R.id.SBContrast);
        SBContrast.setProgress((int) (model.contrast * 100));
        SBContrast.setOnSeekBarChangeListener(this);

        SeekBar SBGamma = findViewById(R.id.SBGamma);
        SBGamma.setProgress((int) (model.gamma * 100));
        SBGamma.setOnSeekBarChangeListener(this);

        SeekBar SBReflection = findViewById(R.id.SBReflection);
        SBReflection.setProgress((int) (model.reflection * 100));
        SBReflection.setOnSeekBarChangeListener(this);

        SeekBar SBSaturation = findViewById(R.id.SBSaturation);
        SBSaturation.setProgress((int) (model.saturation * 100));
        SBSaturation.setOnSeekBarChangeListener(this);
    }

    @SuppressLint("NonConstantResourceId")
    @Override
    public void onProgressChanged(SeekBar seekBar, int value, boolean b) {
        switch (seekBar.getId()) {
            case R.id.SBBrightness:
                model.brightness = value * 0.01f;
                break;
            case R.id.SBContrast:
                model.contrast = value * 0.01f;
                break;
            case R.id.SBGamma:
                model.gamma = value * 0.01f;
                break;
            case R.id.SBReflection:
                model.reflection = value * 0.01f;
                break;
            case R.id.SBSaturation:
                model.saturation = value * 0.01f;
                break;
        }
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
    }
}
