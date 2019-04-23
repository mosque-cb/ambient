(defun execute(socket cmd)
  (progn
    (send socket cmd)
    (print  (recv socket))))

(defun main(socket)
  (progn
    (print  (recv socket))
    (execute  socket (concat 'USER 'SPACE 'anonymous 'ENTER))
    (execute  socket (concat 'PASS 'SPACE  'ENTER))
    (execute  socket (concat 'PASV 'ENTER))
    (execute  socket (concat 'pwd 'ENTER))
    (execute  socket (concat 'system 'ENTER))
    (execute  socket (concat 'QUIT  'ENTER))))


(main (connect  (quote localhost:21)))
