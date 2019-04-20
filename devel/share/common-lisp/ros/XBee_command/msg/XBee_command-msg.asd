
(cl:in-package :asdf)

(defsystem "XBee_command-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :std_msgs-msg
)
  :components ((:file "_package")
    (:file "Temperature" :depends-on ("_package_Temperature"))
    (:file "_package_Temperature" :depends-on ("_package"))
  ))