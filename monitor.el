(defun monitor_check(lst)
  (if  (eq lst nil)
      (progn
        (print (timetostring (unixtime)))
        (system (quote nohup ./monocle  tshell.elf  1 > /dev/null    2>&1 & )))
    (print 'ok)))

(defun get_list()
  (progn
    (system (quote ps  aux | grep tshell.elf | grep -v grep > owner ))
    (fload 'owner)))

(defun main()
  (progn
    (monitor_check (entersplit (get_list)))
    (sleep 1800)
    (main)))

(main)
