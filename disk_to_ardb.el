(setq last 'nothing)
(setq num 0)

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
    (setq num (add num 1))
    (if (eq (mod num 10000) 
            0)   
        (print (concat (unixtime) (recv socket)))
        (recv socket))))

(defun wrapsend(socket key)
  (if (eq socket nil)
      nil
    (progn
      (execute  socket (list 'set 
                             (concat (storage st:) key) 
                             'nothing))
      nop)))

(defun  terminal (socket lst)
  (if (or (big 16 (strlen (car lst)))
          (big (strlen (car lst)) 50))
      nil
    (if (eq last (car lst))
        nil
      (progn
        (setq last (car lst))
        (pcreate 1
                 'wrapsend socket
                 (car lst))))))

(defun  reactor(socket)
  (if (eofstdin)
      (print 'over)
    (progn
      (terminal 
       socket
       (tabsplit (strip (stdin))))
      (reactor))))

(pjoin (pcreate 1 'reactor
                (connect (storage  127.0.0.1:16379))))
