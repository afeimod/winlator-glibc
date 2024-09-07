package com.winlator.eventbus;

public interface ISubscriber {
    boolean onEvent(BaseEvent event);
}
