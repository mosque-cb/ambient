(defun install(event)
  (progn
    (atadd event 
           (add 20  (unixtime))
           (lambda (calc_time) (progn
                                 (print  'case)
                                 (print  calc_time)
                                 (print  (storage storage_time is twenty)))))
    (atadd event 
           (add 7  (unixtime))
           (lambda (calc_time) (progn
                                 (print  'case)
                                 (print calc_time)
                                 (print  (storage storage_time is seven)))))
    (atadd event 
           (add 8  (unixtime))
           (lambda (calc_time) (progn
                                 (print  'case)
                                 (print calc_time)
                                 (print  (storage storage_time is eight)))))
    (atadd event 
           (add 10 (unixtime))  
           (lambda (calc_time) (progn
                                 (print  'case)
                                 (print calc_time)
                                 (print  (storage storage_time is ten)))))
    (atadd event 
           (add 5 (unixtime))  
           (lambda (calc_time) (progn
                                 (print  'case)
                                 (print calc_time)
                                 (print  (storage storage_time is five)))))))

(defun step(event calc_time interval)
  (progn
    (print  'idle)
    (sleep interval)
    (atpoll  event  (list (add calc_time interval)))
    (dispatch event 
              (add calc_time 
                   interval))))

(defun dispatch (event calc_time)
  (step
   event   
   calc_time 
   (print (atwait event))))

(dispatch 
 (install (atcreate))
 0)

(fflush)
