(defun execute(socket cmd)
  (progn
    (send socket cmd)
    (print (recv socket))))

(defun main(socket)
  (progn
    (print  1)
    (execute  socket (spaceconcat '(ubuntu)))
    (print  2)    
    (execute  socket (spaceconcat '(ubuntu)))
    (print  3)    
    (execute  socket (spaceconcat '(ls)))
    (print  4)        
    (execute  socket (print (spaceconcat (list (storage exit) 'LINE))))))

(main (connect  (storage 120.27.95.65:23)))
