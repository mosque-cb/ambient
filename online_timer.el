(defun multi(event tt)
  (progn
    (atadd event 
           (add (unixtime) tt)
           (lambda() (progn
                       (print (concat tt 'SPACE 'SPACE 'SPACE (timestring)))
                       (multi event tt))))))

(defun install(event)
  (progn
    (multi event 20)                        
    (multi event 7)                       
    (multi event 8)                       
    (multi event 10)                       
    (multi event 5)))                       

(defun step(event interval)
  (if (eq interval 0)
      (print 'over)
    (progn
      (print (concat 'idle 'SPACE interval))
      (sleep interval)
      (atpoll  event nil)
      (dispatch event)))) 

(defun dispatch (event)
  (step
   event   
   (atwait event)))

(dispatch 
 (install (atcreate)))

(fflush)










