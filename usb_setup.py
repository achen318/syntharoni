import usb.core
import usb.util

# Find device using VID/PID
dev = usb.core.find(idVendor=0x04B4, idProduct=0x80)
if dev is None:
    raise ValueError("Device not found")

# Set active configuration
dev.set_configuration()

# Find IN and OUT endpoints
intf = dev.get_active_configuration()[(0, 0)]

ep_out = usb.util.find_descriptor(
    intf,
    custom_match=lambda e: usb.util.endpoint_direction(e.bEndpointAddress)
    == usb.util.ENDPOINT_OUT,
)

ep_in = usb.util.find_descriptor(
    intf,
    custom_match=lambda e: usb.util.endpoint_direction(e.bEndpointAddress)
    == usb.util.ENDPOINT_IN,
)

assert ep_out is not None, "OUT endpoint not found"
assert ep_in is not None, "IN endpoint not found"
