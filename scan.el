(defun worker(ip socket)
  (if (eq socket nil)
      nil
    (progn
      (close socket)
      (print ip))))

(defun decide(ip)
  (worker 
   ip
   (connect
    (concat ip
            (storage :) 
            8855))))

(defun  reactor()
  (if (eofstdin)
      nil
    (progn
      (decide (strip (stdin)))
      (reactor))))

(reactor)


