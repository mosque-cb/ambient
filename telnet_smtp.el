(defun execute(socket cmd)
  (progn
    (send socket cmd)
    (print (recv socket))))

(defun smtpmain(socket)
  (progn
    (execute  socket (spaceconcat '(HELO hi LINE)))
    (execute  socket (spaceconcat '(auth login LINE)))
    (execute  socket (print (concat (quote cm9zc2x5bl9kenBAMTI2LmNvbQo=) 'LINE)))
    (execute  socket (print (concat (quote Y3NkbmNzZG4xMzc3NzMK) 'LINE)))
    (execute  socket (print (spaceconcat (list (quote list) 'LINE))))
    (execute  socket (print (spaceconcat (list (quote quit) 'LINE))))))

(smtpmain (connect  (quote smtp.126.com:25)))
