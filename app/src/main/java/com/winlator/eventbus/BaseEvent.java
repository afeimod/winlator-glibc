package com.winlator.eventbus;

import androidx.annotation.NonNull;

public abstract class BaseEvent {
    private EventPriority priority;
    private int handledCounts = 0;
    private String message = "";
    private boolean consumeOnce = false;

    @NonNull
    public EventPriority getPriority() {
        return priority;
    }

    public void setPriority(@NonNull EventPriority priority) {
        this.priority = priority;
    }

    @NonNull
    public String getMessage() {
        return message;
    }

    public void setMessage(@NonNull String msg) {
        message = msg;
    }

    public void setConsumeOnce(boolean once) {
        consumeOnce = once;
    }

    public boolean isConsumeOnce() {
        return consumeOnce;
    }

    public void handled() {
        handledCounts++;
    }

    public int getHandledCounts() {
        return handledCounts;
    }
}
