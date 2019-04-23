(seq last 'nothing)
(seq num 0)

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

(defun worker(source material)
  (if (eq material nil)
      nil
    (if (eq (size material) 3)
        (if (eq (car (cdr material)) 'nothing)
            nil
          (print source))
      (if (eq (size material) 2)
          (print source)
        nil))))

(defun execute(source socket cmd)
  (progn
    (send socket  (format_protocol cmd))
    (seq num (add num 1))
    (worker
     source
     (linesplit (recv socket)))
    (if (eq (mod num 10000) 
            0)   
        (printerr (concat (unixtime) 'SPACE num))
      nil)))

(defun wrapsend(socket key source)
  (if (eq socket nil)
      nil
    (progn
      (execute  source socket (list 'get 
                                    (concat (quote st:) key)))
      nop)))

(defun  terminal (socket lst source)
  (if (or (big 16 (strlen (car lst)))
          (big (strlen (car lst)) 50))
      nil
    (if (eq last (car lst))
        nil
      (progn
        (seq last (car lst))
        (pcreate 1
                 'wrapsend socket
                 (car lst)
                 source
                 )))))

(defun  wrapterminal (socket lst)
  (terminal socket 
            (tabsplit lst)
            lst
            ))

(defun  reactor(socket)
  (if (eofstdin)
      nil
    (progn
      (wrapterminal 
       socket
       (strip (stdin)))
      (reactor))))

(pjoin (pcreate 1 'reactor
                (connect (quote  127.0.0.1:16379))))
