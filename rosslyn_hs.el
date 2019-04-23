(defun  handle (socket)
  (progn
    (send socket (spaceconcat (list 'server 'echo (print (recv  socket)))))
    (close socket)))

(defun  wraphandle (socket)
  (seq 'unfinish (cons (pcreate  1 'handle socket) (geq 'unfinish))))

(defun recycle(lst)
  (if (eq  lst  nil)
      nil
    (progn
      (pjoin  (car lst))
      (recycle (cdr lst)))))

(defun flashx(socket)
  (progn
    (print  'recycle)
    (printchar  'memory)
    (print (geq 'unfinish))
    (recycle (geq 'unfinish))
    (seq 'unfinish nil)
    (dispatch socket (geq 'pool))))

(defun dispatch (socket num)
  (if (eq num 0)
      (flashx socket)
    (progn
      (wraphandle  (accept  socket))
      (sleep 1)
      (dispatch  socket (minus num 1)))))

(seq 'unfinish nil)
(seq 'pool 20)
(pjoin (pcreate 100 'dispatch  (bind  8528 (quote 0.0.0.0)) (geq 'pool)))










