(setq last 'nothing)

(defun wrapsend(socket key)
  (if (eq socket nil)
      nil
    (progn
      (send socket (spaceconcat (list 'set key 1)))
      (printerr (concat (unixtime) (recv socket)))
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
                (connect (storage 127.0.0.1:8778))))
