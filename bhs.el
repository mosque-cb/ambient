(defun  handle (socket)
  (progn
    (send socket (spaceconcat (list 'server 'echo (print (recv  socket)))))
    (sleep 1)
    (close socket)
    nop))

(defun dispatch (socket)
  (progn
    (pcreate 1 'handle  (accept  socket))
    (print 'wating)
    (sleep 1)
    (dispatch  socket)))

(pjoin (pcreate 1 'dispatch  (bind  8528 (storage 0.0.0.0))))










