(defun cadr (lst)
  (car (cdr lst)))

(defun dispatch (event times)
  (dispatch  (aepoll  event (list times))
             (add times 1)))

(defun  com(filesize socket file)
  (if (or (big 512 filesize)
          (eq 512 filesize))
      (progn
        (send socket (fread file filesize))
        (sleep 1)
        (fclose file))
    (progn
      (send socket (fread file 512))
      (com (minus filesize 512)
           socket 
           file ))))

(defun strategy_worker(socket header headsize)
  (send socket  (concat (quote HTTP/1.1 200 OK) 
                        'LINE 
                        header
                        'LINE 
                        (concat (quote Content-Length: ) headsize)
                        'LINE 
                        'LINE)))

(defun genericc(lst)
  (if (eq (size lst) 1)
      nil
    (concat
     (quote <li><a href=)
     'DOUBLE_QUOTE
     (car lst)
     'DOUBLE_QUOTE
     (quote >)
     (car lst)
     (quote </a></li>)
     (genericc (cdr lst)))))

(defun dirshow(socket material)
  (progn
    (strategy_worker socket
                     (quote Content-Type: text/html) 
                     (strlen material))
    (send socket material)))

(defun helper(socket filepath filesize)
  (if (or (eq  filesize 0)
          (eq  filesize (minus 0 1)))
      (progn 
        (system  (concat 'ls  'SPACE  filepath (quote > haha)))
        (dirshow socket
                 (concat
                  (quote  
                   <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
                   <html xmlns="http://www.w3.org/1999/xhtml">
                   <head>
                   <title>apache</title>
                   </head>
                   <body> 
                   <h1>files</h1>)
                  (quote <ul>)
                  (concat 'num 'SPACE 'is 'SPACE (size (entersplit (fload 'haha))))
                  (print (genericc (entersplit (fload 'haha))))
                  (quote </ul>)
                  (quote
                   </body>
                   </html>))))
    (progn
      (if (or (not (eq (find filepath (quote .html)) nil))
              (not (eq (find filepath (quote .c)) nil)))
          (strategy_worker socket
                           (quote Content-Type: text/html) 
                           filesize)
        (strategy_worker socket
                         (quote Content-Type: application/octet-stream)
                         filesize))
      (com
       filesize
       socket
       (fopen filepath 'r)))))

(defun strategy(socket filepath)
  (if (eq (strlen filepath) 0)
      (helper socket 
              (quote ./)
              0)
    (if (eq (find filepath (quote .)) nil)
        (helper socket 
                filepath
                0)
      (helper socket 
              filepath
              (ftell filepath)))))

(defun extractcall(material)
  (strdup material
          1
          (find material 'SPACE)))

(defun extract_path(material)
  (extractcall (strdup material
                     (find material (quote /))
                     (strlen material))))

(defun wrap_strategy(socket lst)
  (progn
    (print lst)
    (if  (not (eq (find (car lst) 'GET) nil))
        (strategy socket 
                  (print (extract_path (car lst))))
      (strategy_worker socket
                       (quote Content-Type: text/html) 
                       (quote  HTTP/1.0 501 Not Implemented)))))

(defun handle(en socket first)
  (aeadd  
   'read
   (lambda (second)   (progn
                        (wrap_strategy socket
                                       (linesplit (recv  socket)))
                        (print (concat 
                                (concat 'first 'SPACE first)
                                'SPACE
                                (concat 'second 'SPACE second)))
                        (sleep 1)
                        (close socket)
                        (aedel 'read 
                               en
                               socket)))
   en
   socket))

(defun init(en socket)
  (progn
    (aeadd  
     'read
     (lambda (first)   (progn
                         (print (concat 'first 'SPACE first))
                         (handle en (accept  socket) first)))
     en 
     socket)))

(dispatch 
 (init  (aecreate 'select)   
        (bind  8634  (quote 0.0.0.0)))
 0)

