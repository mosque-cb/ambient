(defun execute(socket cmd)
  (progn
    (send socket cmd)
    (print (recv socket))))

(defun smtpmain(socket)
  (progn
    (execute  socket (spaceconcat '(HELO hi LINE)))
    (execute  socket (spaceconcat '(auth login LINE)))
    (execute  socket (print (concat (storage cm9zc2x5bl9kenBAMTI2LmNvbQo=) 'LINE)))
    (execute  socket (print (concat (storage Y3NkbmNzZG4xMzc3NzMK) 'LINE)))
    (execute  socket (print (spaceconcat (list (storage list) 'LINE))))
    (execute  socket (print (spaceconcat (list (storage quit) 'LINE))))))

(smtpmain (connect  (storage smtp.126.com:25)))
