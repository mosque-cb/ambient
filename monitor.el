(defun monitor_check(lst)
  (if  (eq lst nil)
      (progn
        (print (timestring))
        (system (storage nohup ./monitor tshell.elf > /dev/null & )))
    (print 'ok)))

(defun get_list()
  (progn
    (system (storage ps  aux | grep tshell.elf | grep -v grep > owner ))
    (fload 'owner)))

(defun main()
  (progn
    (monitor_check (entersplit (get_list)))
    (sleep 1800)
    (main)))

(main)
