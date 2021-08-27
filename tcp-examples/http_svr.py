# -*- coding: utf-8 -*-
import threading
sem = threading.Semaphore()
import uuid

from flask import Flask, jsonify, request
from flask_cors import CORS

app = Flask(__name__)
app.config.from_object(__name__)

CORS(app, resources={r'/*': {'origins': '*'}})

# book inventory
BOOKS = [
    {
        'id': uuid.uuid4().hex,
        'title': 'On the Road',
        'author': 'Jack Kerouac',
        'read': True
    },
    {
        'id': uuid.uuid4().hex,
        'title': 'Harry Potter and the Philosopher\'s Stone',
        'author': 'J. K. Rowling',
        'read': False
    },
    {
        'id': uuid.uuid4().hex,
        'title': 'Green Eggs and Ham',
        'author': 'Dr. Seuss',
        'read': True
    }
]


@app.route('/ping', methods=['GET'])
def ping_pong():
    return jsonify('pong')


@app.route('/books', methods=['GET', 'POST'])
def all_books():
    response_object = {'status': 'success'}

    if request.method == 'POST':
        post_data = request.get_json()

        sem.acquire()
        BOOKS.append({
            'id': uuid.uuid4().hex,
            'title': post_data.get('title'),
            'author': post_data.get('author'),
            'read': post_data.get('read')
        })
        sem.release()

        response_object['message'] = 'Book Added!'
    else:
        sem.acquire()
        response_object['books'] = BOOKS
        sem.release()
    return jsonify(response_object)


@app.route('/books/<book_id>', methods=['PUT', 'DELETE'])
def single_book(book_id):
    response_object = {'status': 'success'}
    if request.method == 'PUT':
        post_data = request.get_json()
        remove_book(book_id)

        sem.acquire()
        BOOKS.append({
            'id': uuid.uuid4().hex,
            'title': post_data.get('title'),
            'author': post_data.get('author'),
            'read': post_data.get('read')
        })
        sem.release()

        response_object['message'] = 'Book Updated!'
    if request.method == 'DELETE':
        remove_book(book_id)
        response_object['message'] = 'Book Removed!'
    return jsonify(response_object)


def remove_book(book_id):
    sem.acquire()
    for book in BOOKS:
        if book['id'] == book_id:
            BOOKS.remove(book)
            return True
    sem.release()
    return False


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=18988, debug=True)
