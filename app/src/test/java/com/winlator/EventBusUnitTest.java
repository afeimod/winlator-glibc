package com.winlator;

import com.winlator.eventbus.BaseEvent;
import com.winlator.eventbus.EventBus;
import com.winlator.eventbus.EventPriority;
import com.winlator.eventbus.ISubscriber;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import java.lang.reflect.Field;
import java.util.HashMap;
import java.util.Objects;
import java.util.Set;
import java.util.concurrent.Future;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class EventBusUnitTest {
    private static final long POST_WAIT_TIME = 100;
    private MyEventBus mEventBus;

    @Before
    public void setUp() {
        mEventBus = null;

        try {
            mEventBus = new MyEventBus();
        } catch (Exception e) {
            Assert.fail();
        }

        mEventBus.start();
    }

    @After
    public void tearDown() {
        mEventBus.destroy();
    }

    @Test
    public void test_eventbus_register() {
        ISubscriber subscriber1 = new MySubscriber1();
        ISubscriber subscriber2 = new MySubscriber2();
        ISubscriber subscriber3 = new MySubscriber3();

        // Test register event
        Assert.assertTrue(mEventBus.register(subscriber1, MyEvent1.class));
        Assert.assertTrue(mEventBus.register(subscriber2, MyEvent2.class));
        Assert.assertTrue(mEventBus.register(subscriber3, MyEvent1.class, MyEvent2.class));
        Assert.assertEquals(mEventBus.mMap.size(), 3);
        Assert.assertEquals(Objects.requireNonNull(mEventBus.mMap.get(subscriber3)).size(), 2);

        // Test register with duplicated event
        Assert.assertFalse(mEventBus.register(subscriber1, MyEvent1.class));
        Assert.assertFalse(mEventBus.register(subscriber3, MyEvent2.class));

        // Test unregister with unsubscribed event
        Assert.assertFalse(mEventBus.unregister(subscriber1, MyEvent2.class));
        Assert.assertEquals(mEventBus.mMap.size(), 3);

        // Test unregister event
        Assert.assertTrue(mEventBus.unregister(subscriber1, MyEvent1.class));
        Assert.assertEquals(mEventBus.mMap.size(), 2);

        // Test unregister with unregistered object
        Assert.assertFalse(mEventBus.unregister(subscriber1));

        // Test unregister one event
        Assert.assertTrue(mEventBus.unregister(subscriber3, MyEvent1.class));
        Assert.assertEquals(mEventBus.mMap.size(), 2);
        Assert.assertEquals(Objects.requireNonNull(mEventBus.mMap.get(subscriber3)).size(), 1);

        // Test unregister events
        Assert.assertTrue(mEventBus.register(subscriber3, MyEvent1.class));
        Assert.assertEquals(Objects.requireNonNull(mEventBus.mMap.get(subscriber3)).size(), 2);
        Assert.assertTrue(mEventBus.unregister(subscriber3, MyEvent1.class, MyEvent2.class));
        Assert.assertEquals(mEventBus.mMap.size(), 1);
    }

    @Test
    public void test_eventbus_post() throws InterruptedException {
        String msg1 = "event1";
        String msg2 = "event2";
        String msg3 = "event3";
        String msg4 = "event4";
        BaseEvent event1 = new MyEvent1();
        BaseEvent event2 = new MyEvent2();
        BaseEvent event3 = new MyEvent1();
        BaseEvent event4 = new MyEvent1();
        MySubscriber1 subscriber1 = new MySubscriber1();
        MySubscriber2 subscriber2 = new MySubscriber2();
        MySubscriber3 subscriber3 = new MySubscriber3();

        event1.setMessage(msg1);
        event2.setMessage(msg2);
        event2.setConsumeOnce(true);
        event3.setMessage(msg3);
        event3.setPriority(EventPriority.NORMAL);
        event4.setMessage(msg4);
        event4.setPriority(EventPriority.HIGH);

        mEventBus.register(subscriber1, MyEvent1.class);
        mEventBus.register(subscriber2, MyEvent2.class);
        mEventBus.register(subscriber3, MyEvent1.class, MyEvent2.class);

        // Test post event1, which is a default event
        Assert.assertTrue(mEventBus.post(event1));
        Thread.sleep(POST_WAIT_TIME);
        Assert.assertEquals(event1.getHandledCounts(), 2);
        Assert.assertEquals(subscriber1.msg, msg1);
        Assert.assertNull(subscriber2.msg);
        Assert.assertEquals(subscriber3.msg1, msg1);

        // Test post event2, which will be only consumed once
        Assert.assertTrue(mEventBus.post(event2));
        Thread.sleep(POST_WAIT_TIME);
        Assert.assertEquals(event2.getHandledCounts(), 1);
        Assert.assertNotEquals(subscriber2.msg, subscriber3.msg2);
        Assert.assertTrue(subscriber2.msg.equals(msg2) || subscriber3.msg2.equals(msg2));

        // Test post two events that have different priority
        synchronized (mEventBus.lock) {
            // post event that has low priority first
            // in this case, event4 will consumed first, and event3 next
            mEventBus.post(event3);
            mEventBus.post(event4);
        }
        Thread.sleep(POST_WAIT_TIME);
        Assert.assertEquals(event3.getHandledCounts(), 2);
        Assert.assertEquals(event4.getHandledCounts(), 2);
        Assert.assertEquals(subscriber1.msg, msg3);
        Assert.assertEquals(subscriber3.msg1, msg3);
    }

    private static class MyEvent1 extends BaseEvent {

    }

    private static class MyEvent2 extends BaseEvent {

    }

    private static class MySubscriber1 implements ISubscriber {
        public String msg = null;

        @Override
        public boolean onEvent(BaseEvent event) {
            Class<?> eventType = event.getClass();
            if (eventType == MyEvent1.class) {
                msg = event.getMessage();
                return true;
            }
            return false;
        }
    }

    private static class MySubscriber2 implements ISubscriber {
        public String msg = null;

        @Override
        public boolean onEvent(BaseEvent event) {
            Class<?> eventType = event.getClass();
            if (eventType == MyEvent2.class) {
                msg = event.getMessage();
                return true;
            }
            return false;
        }
    }

    private static class MySubscriber3 implements ISubscriber {
        public String msg1 = null;
        public String msg2 = null;

        @Override
        public boolean onEvent(BaseEvent event) {
            Class<?> eventType = event.getClass();
            if (eventType == MyEvent1.class) {
                msg1 = event.getMessage();
                return true;
            } else if (eventType == MyEvent2.class) {
                msg2 = event.getMessage();
                return true;
            }
            return false;
        }
    }

    private static class MyEventBus extends EventBus {
        private LinkedBlockingDeque<BaseEvent> mQueue;
        private HashMap<ISubscriber, Set<Class<? extends BaseEvent>>> mMap;
        private Runnable eventLoop;
        private Future<?> mFuture;
        private ThreadPoolExecutor mThreadPoolExecutor;
        private Object tmpLock;
        public final Object lock;

        public MyEventBus() throws NoSuchFieldException, IllegalAccessException {
            super();
            accessPrivateFieldFromParent();
            lock = tmpLock;
            initExecutor();
        }

        @Override
        public void start() {
            if (mFuture == null) {
                mFuture = mThreadPoolExecutor.submit(eventLoop);
                mQueue.clear();
                mMap.clear();
            }
        }

        @Override
        public void destroy() {
            if (mFuture != null)
                mFuture.cancel(true);

            mQueue.clear();
            mMap.clear();
            mFuture = null;
        }

        @SuppressWarnings("unchecked")
        private void accessPrivateFieldFromParent() throws NoSuchFieldException, NullPointerException, IllegalAccessException {
            Class<?> parentClass = this.getClass().getSuperclass();
            assert parentClass != null;
            mQueue = (LinkedBlockingDeque<BaseEvent>) accessField(parentClass, "mQueue");
            mMap = (HashMap<ISubscriber, Set<Class<? extends BaseEvent>>>) accessField(parentClass, "mMap");
            eventLoop = (Runnable) accessField(parentClass, "eventLoop");
            tmpLock = accessField(parentClass, "lock");
        }

        private Object accessField(Class<?> _class, String fieldName) throws NoSuchFieldException, NullPointerException, IllegalAccessException {
            Field field = _class.getDeclaredField(fieldName);
            field.setAccessible(true);
            return field.get(this);
        }

        private void initExecutor() {
            mThreadPoolExecutor = new ThreadPoolExecutor(0, 8, 60,
                    TimeUnit.SECONDS, new LinkedBlockingQueue<>(), new ThreadPoolExecutor.CallerRunsPolicy());
        }
    }
}
