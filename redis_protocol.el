(defun format_item(lst)
  (if  (eq lst nil)
      nil
    (concat
     (concat '$ 
             (strlen (car lst)) 
             'LINE 
             (car lst) 
             'LINE)
     (format_item (cdr lst)))))

(defun format_protocol(cmd)
  (concat
   '*   
   (size cmd) 
   'LINE
   (format_item cmd)))

(defun execute(socket cmd)
  (progn
    (send socket  (format_protocol cmd))
    (print (recv socket 1000000))))

(defun main(socket)
  (progn
    (execute  socket '(rpush rosslyn abc))
    (execute  socket '(rpush rosslyn def))
    (execute  socket '(rpush rosslyn ghk))
    (execute  socket '(lpop  rosslyn))
    (execute  socket '(lpop  rosslyn))
    (execute  socket '(lpop  rosslyn))
    (execute  socket '(lpop  rosslyn))))


(main (connect  (storage 127.0.0.1:6379)))
