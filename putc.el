(defun  com(filesize socket file)
  (if (or (big 512 filesize)
          (eq 512 filesize))
      (progn
        (send socket (fread file filesize))
        (sleep 2)
        (fclose file))
    (progn
      (send socket (fread file 512))
      (com (minus filesize 512)
           socket 
           file ))))

(defun strategy(socket filename filesize)
  (if(eq  filesize (minus 0 1))
      (send socket   (concat (storage HTTP/1.1 404 NOT FOUND) 'LINE 'LINE))
    (progn
      (if (not (eq (find filename (storage .html)) nil))
          (send socket  (concat (storage HTTP/1.1 200 OK) 'LINE (storage Content-Type: text/html) 'LINE (storage Content-Length: )))
        (if (not (eq (find filename (storage .png)) nil))
            (send socket  (concat (storage HTTP/1.1 200 OK) 'LINE (storage Content-Type: image/png) 'LINE (storage Content-Length: )))
          (send socket  (concat (storage HTTP/1.1 200 OK) 'LINE (storage Content-Type: application/octet-stream) 'LINE (storage Content-Length: )))))
      (progn
        (send socket   (concat (itoa filesize) 'LINE 'LINE))
        (com
         filesize
         socket
         (fopen filename 'r))))))

(defun  remote(filename res socket filesize)
  (progn
    (print 'res)
    (print res)
    (if (eq res 'ok)
        (strategy socket filename filesize)
      (progn
        (print 'remotefailed)
        (print res)
        (close socket)))))

(defun  handle(filename socket)
  (progn
    (send  socket filename)
    (remote filename
            (recv socket 1000000)     
            socket
            (ftell filename))))

(defun  worker()
  (progn
    (print (storage please input the filename:))
    (handle 
     (strip (stdin))
     (connect (storage 0.0.0.0:8778)))))

(worker)
(print 'done)
(sleep 1000)
