(defun execute(socket cmd)
  (progn
    (send socket cmd)
    (print (recv socket))))

(defun main(socket)
  (progn
    (execute  socket (spaceconcat '(guest LINE)))
    (execute  socket 'LINE)
    (execute  socket (print (spaceconcat (list (storage exit) 'LINE))))))

(main (connect  (storage bbs.pku.edu.cn:23)))
