(defun status(dict queue start end)
  (if (big  (size (lrange dict
                         queue 
                         start 
                         end))
            60)
      (progn
        (print 'eject)
        (lpop dict 
              queue))
    (lrange dict          
            queue
            start
            end)))

(defun dispatch(dict)
  (progn
    (sleep (random 60))
    (rpush dict 
           (itoa (minute))
           (timetostring (unixtime)))
    (print (status dict 
                   (itoa (minute))
                   0 
                   100))
    (dispatch dict )))

(dispatch (dcreate))


