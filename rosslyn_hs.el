(defun  handle (socket)
  (progn
    (send socket (spaceconcat (list 'server 'echo (print (recv  socket)))))
    (close socket)))

(defun  wraphandle (socket)
  (setq unfinish (cons (pcreate  1 'handle socket)  unfinish)))

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
    (print unfinish)
    (recycle unfinish)
    (setq unfinish nil)
    (dispatch socket pool)))

(defun dispatch (socket num)
  (if (eq num 0)
      (flashx socket)
    (progn
      (wraphandle  (accept  socket))
      (sleep 1)
      (dispatch  socket (minus num 1)))))

(setq unfinish nil)
(setq pool 20)
(pjoin (pcreate 100 'dispatch  (bind  8528 (storage 0.0.0.0)) pool))










