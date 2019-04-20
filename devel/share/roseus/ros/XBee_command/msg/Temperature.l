;; Auto-generated. Do not edit!


(when (boundp 'XBee_command::Temperature)
  (if (not (find-package "XBEE_COMMAND"))
    (make-package "XBEE_COMMAND"))
  (shadow 'Temperature (find-package "XBEE_COMMAND")))
(unless (find-package "XBEE_COMMAND::TEMPERATURE")
  (make-package "XBEE_COMMAND::TEMPERATURE"))

(in-package "ROS")
;;//! \htmlinclude Temperature.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass XBee_command::Temperature
  :super ros::object
  :slots (_header _data ))

(defmethod XBee_command::Temperature
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:data __data) 0.0)
    )
   (send-super :init)
   (setq _header __header)
   (setq _data (float __data))
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:data
   (&optional __data)
   (if __data (setq _data __data)) _data)
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; float64 _data
    8
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; float64 _data
       (sys::poke _data (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; float64 _data
     (setq _data (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;;
   self)
  )

(setf (get XBee_command::Temperature :md5sum-) "e6c99c37e6f9fe98e071d524cc164e65")
(setf (get XBee_command::Temperature :datatype-) "XBee_command/Temperature")
(setf (get XBee_command::Temperature :definition-)
      "Header header
float64 data
================================================================================
MSG: std_msgs/Header
# Standard metadata for higher-level stamped data types.
# This is generally used to communicate timestamped data 
# in a particular coordinate frame.
# 
# sequence ID: consecutively increasing ID 
uint32 seq
#Two-integer timestamp that is expressed as:
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
# time-handling sugar is provided by the client library
time stamp
#Frame this data is associated with
# 0: no frame
# 1: global frame
string frame_id

")



(provide :XBee_command/Temperature "e6c99c37e6f9fe98e071d524cc164e65")

