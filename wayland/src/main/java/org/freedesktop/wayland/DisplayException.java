package org.freedesktop.wayland;

public class DisplayException extends RuntimeException {
    public DisplayException() {
        super();
    }

    public DisplayException(String message) {
        super(message);
    }
}
