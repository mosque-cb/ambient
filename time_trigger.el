(defun trigger(tt time)
  (progn
    (print (concat 'trigger
                   'SPACE 
                   'over
                   'SPACE 
                   tt
                   'SPACE 
                   (timetostring time)
                   'SPACE 
                   'SPACE 
                   (timetostring (unixtime))))
    nop))

(defun wrap_trig(tt unixt)
  (progn
    (print (concat 'trigger 'SPACE tt 'SPACE (timetostring unixt)))
    unixt))

(defun multi(event tt)
  (atadd event 
         (add (unixtime) tt)
         (lambda() (progn
                     (pcreate 1 'trigger tt (wrap_trig tt
                                                       (add (unixtime) tt)))
                     (multi event tt)))))

(defun install(event)
  (progn
    (multi event 20)
    (multi event 7)
    (multi event 2)
    (multi event 8)
    (multi event 1)
    (multi event 10)
    (multi event 5)))

(defun step(event interval)
  (if (eq interval 0)
      (print 'over)
    (progn
      (sleep interval)
      (atpoll  event nil)
      (dispatch event)))) 

(defun dispatch (event)
  (step
   event   
   (atwait event)))

(pjoin (pcreate 1 'dispatch 
                (install (atcreate))))
