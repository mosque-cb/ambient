(defun format_protocol (cmd)
cmd)

(defun execute(socket cmd)
  (progn
    (send socket  (format_protocol cmd))
    (print (recv socket))))

(defun main(socket)
  (progn
    (execute  socket 'stat)
    (execute  socket 'ruok)
    (execute  socket 'dump)
    (execute  socket (quote  ls /))
    (execute  socket 'conf)))

(main (connect (quote 127.0.0.1:2181)))
