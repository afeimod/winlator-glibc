package com.winlator.eventbus;

import androidx.annotation.NonNull;

import com.winlator.applications.WinlatorApplication;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.Future;
import java.util.concurrent.LinkedBlockingDeque;

public class EventBus {
    private final LinkedBlockingDeque<BaseEvent> mQueue = new LinkedBlockingDeque<>();
    private final HashMap<ISubscriber, Set<Class<? extends BaseEvent>>> mMap = new HashMap<>();
    private Future<?> mFuture;
    private final Object lock = new Object();

    @SafeVarargs
    public final boolean register(@NonNull ISubscriber subscriber, @NonNull Class<? extends BaseEvent>... eventTypes) {
        synchronized (lock) {
            var eventTypeSets = mMap.computeIfAbsent(subscriber, k -> new HashSet<>());
            boolean ret = true;

            for (Class<? extends BaseEvent> eventType : eventTypes) {
                if (!eventTypeSets.add(eventType))
                    ret = false;
            }
            return ret;
        }
    }

    public boolean unregister(@NonNull ISubscriber subscriber) {
        synchronized (lock) {
            return mMap.remove(subscriber) != null;
        }
    }

    @SafeVarargs
    public final boolean unregister(@NonNull ISubscriber subscriber, @NonNull Class<? extends BaseEvent>... eventTypes) {
        var eventTypeSets = mMap.get(subscriber);
        boolean ret = true;

        if (eventTypeSets == null)
            return false;

        synchronized (lock) {
            for (Class<? extends BaseEvent> eventType : eventTypes) {
                if (!eventTypeSets.remove(eventType))
                    ret = false;
            }

            if (eventTypeSets.isEmpty())
                mMap.remove(subscriber);
        }

        return ret;
    }

    public boolean post(@NonNull BaseEvent event) {
        try {
            if (event.getPriority() == EventPriority.HIGH)
                mQueue.putFirst(event);
            else
                mQueue.put(event);
        } catch (InterruptedException e) {
            return false;
        }
        return true;
    }

    private void dispatchEvent(@NonNull BaseEvent event) {
        ISubscriber subscriber;
        Set<Class<? extends BaseEvent>> sets;
        boolean ret;

        for (var entry : mMap.entrySet()) {
            subscriber = entry.getKey();
            sets = entry.getValue();

            if (sets.contains(event.getClass())) {
                ret = subscriber.onEvent(event);
                if (ret) {
                    event.handled();
                    if (event.isConsumeOnce())
                        break;
                    else
                        continue;
                }
            }
        }
    }

    private final Runnable eventLoop = () -> {
        BaseEvent currentEvent;

        while (true) {
            try {
                currentEvent = mQueue.take();
            } catch (InterruptedException e) {
                break;
            }
            synchronized (lock) {
                dispatchEvent(currentEvent);
                // Explicitly setting currentEvent to null
                // since LinkedBlockingDeque may blocking the thread for a long time
                currentEvent = null;
            }
        }
    };

    public void destroy() {
        if (mFuture != null)
            mFuture.cancel(true);

        mQueue.clear();
        mMap.clear();
        mFuture = null;
    }

    public void start() {
        if (mFuture == null) {
            mFuture = WinlatorApplication.getInstance().getExecutor().submit(eventLoop);
            mQueue.clear();
            mMap.clear();
        }
    }
}
