(defun wraphelper(label line)
  (if (eq label nil)
      nil
    (print (strdup line
                   (print label)
                   (print (strlen line))))))

(defun helper(line pattern)
  (wraphelper (grep line pattern)
              line))

(defun  reactor()
  (if (eofstdin)
      nil
    (progn
      (helper (stdin) (storage chen.*g))
      (reactor))))

(reactor)
