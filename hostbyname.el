(defun  reactor()
  (if (eofstdin)
      nil
    (progn
      (print (dns (strip (stdin))))
      (reactor))))

(reactor)
