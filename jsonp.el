(defun cadr (lst)
  (car (cdr lst)))

(defun genematerial(echo)
  (jtosx (jaddobject
          (jaddobject (jcreate) 
                      'value
                      (jcreatestring  echo))
          'code
          (jcreatestring  'jsonp))))

(defun dispatch (event times)
  (dispatch  (aepoll  event (list times))
             (add times 1)))

(defun strategy_worker(socket header material)
  (progn
    (send socket  (concat (quote HTTP/1.1 200 OK) 
                          'LINE 
                          header
                          'LINE 
                          (concat (quote Content-Length: ) (strlen material))
                          'LINE 
                          'LINE))
    (send socket (print material))))

(defun wrap_rtn(func)
  (if (and (eq (size func) 2)
           (eq (car func) 'callback))
      (concat (cadr func) 
              'CURVEL 
              (genematerial (timetostring (unixtime)))
              'CURVER)
    (concat 'alert
            'CURVEL 
            (genematerial 'error) 
            'CURVER)))        

(defun strategy(socket status dynamic)
  (if (eq status 'page)
      (progn
        (strategy_worker socket
                         (quote Content-Type: text/html) 
                         (concat
                          (quote  
                           <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
                           <html xmlns="http://www.w3.org/1999/xhtml">
                           <head>
                           <title></title>
                           <script type="text/javascript">
                           var )
                          dynamic
                          (quote = function(data) {
                                   alert('value:' + data.value + '  code:' + data.code) ;
                                   }    ;  
                                   var url =)
                          (concat 'DOUBLE_QUOTE
                                  (quote http://0.0.0.0:8634?callback=) 
                                  dynamic
                                  'DOUBLE_QUOTE)
                          (quote      ; 
                           var script = document.createElement('script') ;
                           script.setAttribute('src', url) ; 
                           document.getElementsByTagName('head')[0].appendChild(script) ;
                           </script>
                           </head>
                           <body> 
                           <h1>jsonp test, love you</h1> 
                           </body>
                           </html>))))
    (progn
      (strategy_worker socket 
                       (quote Content-Type: text/javascript) 
                       (wrap_rtn (split status (quote =)))))))

(defun extractcall(material)
  (print (strdup material
                 1
                 (find material 'SPACE))))

(defun extract_status(material)
  (if (eq (find material (quote ?)) nil)
      'page
    (extractcall (strdup material
                       (find material (quote ?))
                       (strlen material)))))

(defun wrap_strategy(socket lst)
  (progn
    (print lst)
    (if  (not (eq (find (car lst) 'GET) nil))
        (strategy socket 
                  (print (extract_status (car lst)))
                  (randomname))
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
                        (sleep 2)
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

