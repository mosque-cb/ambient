(defun execute(socket cmd)
  (progn
    (send socket (spaceconcat cmd))
    (print (recv socket))))

(defun main(socket)
  (progn
    (execute  socket '(rpush aaaaa abc LINE))
    (execute  socket '(lpop  aaaaa LINE))
    (execute  socket '(lpop  aaaaa LINE))))


(main (connect  (storage 127.0.0.1:8380)))
